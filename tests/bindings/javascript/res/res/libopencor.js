let fileManager = null;
let file = null;
let document = null;
let simulation = null;
let instance = null;
let instanceTask = null;
const { lightningChart } = lcjs;
const chart = lightningChart()
  .ChartXY({
    container: $("#plottingArea")[0],
  })
  .setTitle("")
  .setAnimationsEnabled(false);
const lineSeries = chart.addLineSeries().setName("");

export function showPage(page) {
  $(".nav-link").each(function () {
    if (this.id === "nav" + page) {
      this.classList.add("active");
    } else {
      this.classList.remove("active");
    }
  });

  $(".page").each(function () {
    this.style.display = this.id === "page" + page ? "block" : "none";
  });
}

function showError(error) {
  if (!error.endsWith(".")) {
    error += ".";
  }

  $("#errorMessage").html(error);

  updateFileUi(false, false, true, true, false);
}

function listFiles() {
  console.log("-------------------");

  if (fileManager.hasFiles) {
    console.log("Files:");

    const files = fileManager.files;

    for (let i = 0; i < files.size(); ++i) {
      console.log(" - " + files.get(i).fileName);
    }
  } else {
    console.log("No files.");
  }
}

function updateFileUi(
  fileInfoDisplay,
  fileIssuesDisplay,
  fileErrorDisplay,
  resetButtonDisplay,
  simulationDisplay,
) {
  $("#fileInfo").css("display", fileInfoDisplay ? "block" : "none");
  $("#fileIssues").css("display", fileIssuesDisplay ? "block" : "none");
  $("#fileError").css("display", fileErrorDisplay ? "block" : "none");
  $("#reset").css("display", resetButtonDisplay ? "block" : "none");
  $("#simulation").css("display", simulationDisplay ? "block" : "none");

  listFiles();
}

function resetObjects() {
  if (document) {
    document.delete();

    document = null;
  }

  if (instance) {
    instance.delete();

    instance = null;
  }
}

export function reset() {
  const files = fileManager.files;

  for (let i = 0; i < fileManager.fileCount; ++i) {
    files.get(i).delete();
  }

  fileManager.reset();

  resetObjects();

  updateFileUi(false, false, false, false, false);
}

function addAxisElement(axis, name) {
  axis.append("<option>" + name + "</option>");
}

function populateAxis(axisId) {
  const axis = $("#" + axisId);

  axis.empty();

  addAxisElement(axis, instanceTask.voiName);

  for (let i = 0; i < instanceTask.stateCount; ++i) {
    addAxisElement(axis, instanceTask.stateName(i));
    addAxisElement(axis, instanceTask.rateName(i));
  }

  for (let i = 0; i < instanceTask.constantCount; ++i) {
    addAxisElement(axis, instanceTask.constantName(i));
  }

  for (let i = 0; i < instanceTask.computedConstantCount; ++i) {
    addAxisElement(axis, instanceTask.computedConstantName(i));
  }

  for (let i = 0; i < instanceTask.algebraicCount; ++i) {
    addAxisElement(axis, instanceTask.algebraicName(i));
  }
}

export function run() {
  // Reset the plotting area (in case we have some simulation results).

  lineSeries.clear();

  // Retrieve the duration of the simulation and the number of steps.

  simulation.outputEndTime = $("#endingPoint").val();
  simulation.numberOfSteps =
    $("#endingPoint").val() / $("#pointInterval").val();

  // Run the simulation.

  console.log("-------------------");
  console.time("Computing time");
  instance.run();
  console.timeEnd("Computing time");

  // Plot the results.

  console.time("Plotting time");
  updatePlottingAreaAndAxesInfo();
  console.timeEnd("Plotting time");
}

function axisInfo(index) {
  if (index === 0) {
    return [instanceTask.voiAsArray, instanceTask.voiUnit];
  } else if (index <= 2 * instanceTask.stateCount) {
    if (index % 2 !== 0) {
      const ndx = (index - 1) / 2;

      return [instanceTask.stateAsArray(ndx), instanceTask.stateUnit(ndx)];
    } else {
      const ndx = index / 2 - 1;

      return [instanceTask.rateAsArray(ndx), instanceTask.rateUnit(ndx)];
    }
  } else {
    const ndx = index - 1 - 2 * instanceTask.stateCount;

    return [instanceTask.variableAsArray(ndx), instanceTask.variableUnit(ndx)];
  }
}

export function updatePlottingAreaAndAxesInfo() {
  lineSeries.clear();

  const [xAxisArray, xAxisUnit] = axisInfo($("#xAxis").prop("selectedIndex"));
  const [yAxisArray, yAxisUnit] = axisInfo($("#yAxis").prop("selectedIndex"));

  lineSeries.addArraysXY(xAxisArray, yAxisArray);

  $("#xAxisUnit").text(xAxisUnit);
  $("#yAxisUnit").text(yAxisUnit);

  chart.getDefaultAxisX().fit();
  chart.getDefaultAxisY().fit();
}

function formattedIssueDescription(issue) {
  return issue.charAt(0).toLowerCase() + issue.slice(1);
}

// Make sure that the DOM is loaded before we do anything else.

import libOpenCOR from "../libopencor.js";

$(() => {
  // Make sure that libOpenCOR is loaded before we do anything else.

  libOpenCOR().then((loc) => {
    // Keep track of the file manager.

    fileManager = loc.FileManager.instance();

    // Simulation page.

    const input = $("#dropAreaInput")[0];
    const dropArea = $("#dropArea");
    let hasValidFile = false;

    input.onchange = () => {
      if (hasValidFile) {
        let inputFile = input.files[0];
        let fileReader = new FileReader();

        input.value = ""; // Allow the user to select the same file again.

        fileReader.readAsArrayBuffer(inputFile);

        fileReader.onload = async () => {
          try {
            // Retrieve the contents of the file.

            resetObjects();

            const fileArrayBuffer = await inputFile.arrayBuffer();
            const memPtr = loc._malloc(inputFile.size);
            const mem = new Uint8Array(
              loc.HEAPU8.buffer,
              memPtr,
              inputFile.size,
            );

            mem.set(new Uint8Array(fileArrayBuffer));

            file = new loc.File(inputFile.name);

            file.setContents(memPtr, inputFile.size);

            // Determine the type of the file.

            let fileType = "some unknown file";
            let knownFile = true;

            if (file.type === loc.File.Type.CELLML_FILE) {
              fileType = "a CellML file";
            } else if (file.type === loc.File.Type.SEDML_FILE) {
              fileType = "a SED-ML file";
            } else if (file.type === loc.File.Type.COMBINE_ARCHIVE) {
              fileType = "a COMBINE archive";
            } else {
              knownFile = false;
            }

            $("#fileName").html(inputFile.name);
            $("#fileType").html(fileType);

            // Display any issues with the file or run it.

            let hasIssues = false;

            if (knownFile) {
              if (file.hasIssues) {
                const issuesElement = $("#issues");
                const fileIssues = file.issues;

                issuesElement.empty();

                for (let i = 0; i < fileIssues.size(); ++i) {
                  const issue = fileIssues.get(i);

                  issuesElement.append(
                    '<li><span class="bold">' +
                      issue.typeAsString +
                      ":</span> " +
                      formattedIssueDescription(issue.description) +
                      "</li>",
                  );
                }

                hasIssues = true;
              } else {
                // Retrieve some information about the simulation.

                document = new loc.SedDocument(file);
                simulation = document.simulations.get(0);
                instance = document.instantiate();
                instanceTask = instance.tasks.get(0);

                $("#endingPoint").val(simulation.outputEndTime);
                $("#endingPointUnit").text(instanceTask.voiUnit);

                $("#pointInterval").val(
                  simulation.outputEndTime / simulation.numberOfSteps,
                );
                $("#pointIntervalUnit").text(instanceTask.voiUnit);

                // Populate the X and Y axis dropdown lists.

                populateAxis("xAxis");
                populateAxis("yAxis");

                // By default we plot the first state variable against the VOI.

                $("#xAxis").val(instanceTask.voiName);
                $("#yAxis").val(instanceTask.stateName(0));

                // Update the plotting area (in case we have some simulation results and we drop a new file) and the
                // axes information.

                updatePlottingAreaAndAxesInfo();
              }
            }

            updateFileUi(true, hasIssues, false, true, knownFile && !hasIssues);
          } catch (exception) {
            showError(exception.message);
          }
        };

        fileReader.onerror = () => {
          showError(fileReader.error.message);
        };
      } else {
        updateFileUi(false, false, false, false, false);
      }
    };

    ["dragenter", "dragover"].forEach((eventName) => {
      dropArea[0].addEventListener(eventName, (event) => {
        if (event.dataTransfer.items.length === 1) {
          hasValidFile = true;

          dropArea.addClass("drop-area-valid-file");
        } else {
          hasValidFile = false;

          dropArea.addClass("drop-area-invalid-file-s");
        }
      });
    });

    ["dragleave", "drop"].forEach((eventName) => {
      dropArea.on(eventName, () => {
        dropArea.removeClass("drop-area-valid-file");
        dropArea.removeClass("drop-area-invalid-file-s");
      });
    });

    // Versions page.

    $("#OpenCOR").html(loc.versionString());
    $("#Clang").html(loc.clangVersionString());
    $("#libCellML").html(loc.libcellmlVersionString());
    $("#libCOMBINE").html(loc.libcombineVersionString());
    $("#libSEDML").html(loc.libsedmlVersionString());
    $("#LLVM").html(loc.llvmVersionString());
    $("#SUNDIALS").html(loc.sundialsVersionString());

    // Start with our simulation page.

    showPage("Simulation");
  });
});
