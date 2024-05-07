let sed = null;
let simulation = null;
let instance = null;
const { lightningChart } = lcjs;
const lc = lightningChart({
  license:
    "0002-nzAQF3zqMCpblLS99rf02G/6gxAtKwAxEC5o8jYpT4yyvi4PLN2GbqtlRwIftmLnHvzQLnGyUSxM3ZeY/K0T8CYy-MEUCIGFCtwYUZqBfv+B7Lu63gSSRGgNZ+XjiFIrVTIUzFYrPAiEAq8ycNenFAtDe4FEgMRaiR7qZSkoLp0mvXbtdOLhZ+0A=",
  licenseInformation: {
    appTitle: "LightningChart JS Trial",
    company: "LightningChart Ltd.",
  },
});
const chart = lc
  .ChartXY({
    container: $("#plottingArea")[0],
  })
  .setTitle("")
  .setAnimationsEnabled(false);
const lineSeries = chart.addLineSeries().setName("");

export function showPage(page) {
  document.querySelectorAll(".nav-link").forEach((crtPage) => {
    if (crtPage.id === "nav" + page) {
      crtPage.classList.add("active");
    } else {
      crtPage.classList.remove("active");
    }
  });

  document.querySelectorAll(".page").forEach((crtPage) => {
    crtPage.style.display = crtPage.id === "page" + page ? "block" : "none";
  });
}

function showError(error) {
  if (!error.endsWith(".")) {
    error += ".";
  }

  $("#errorMessage").html(error);

  updateFileUi(false, false, true, true);
}

function updateFileUi(
  fileInfoDisplay,
  fileIssuesDisplay,
  fileErrorDisplay,
  resetButtonDisplay
) {
  $("#fileInfo").css("display", fileInfoDisplay ? "block" : "none");
  $("#fileIssues").css("display", fileIssuesDisplay ? "block" : "none");
  $("#fileError").css("display", fileErrorDisplay ? "block" : "none");
  $("#resetFile").css("display", resetButtonDisplay ? "block" : "none");
  $("#simulation").css(
    "display",
    fileInfoDisplay && !fileIssuesDisplay ? "block" : "none"
  );
}

export function resetFile() {
  $("#dropAreaInput").value = "";

  updateFileUi(false, false, false, false);
}

function addAxisElement(axis, name) {
  if (name === "--") {
    axis.append("<hr>");
  } else {
    axis.append("<option>" + name + "</option>");
  }
}

function populateAxis(axisId) {
  const axis = $("#" + axisId);

  axis.empty();

  addAxisElement(axis, instance.voiName());
  addAxisElement(axis, "--");

  for (let i = 0; i < instance.stateCount(); ++i) {
    addAxisElement(axis, instance.stateName(i));
    addAxisElement(axis, instance.rateName(i));
  }

  for (let i = 0; i < instance.variableCount(); ++i) {
    addAxisElement(axis, instance.variableName(i));
  }
}

export function run() {
  console.time("Elapsed time");
  instance.run();
  console.timeEnd("Elapsed time");

  changeAxis();
}

function axisArray(index) {
  if (index === 0) {
    return instance.voi();
  } else if (index <= 2 * instance.stateCount()) {
    if (index % 2 !== 0) {
      return instance.state((index - 1) / 2);
    } else {
      return instance.rate(index / 2 - 1);
    }
  } else {
    return instance.variable(index - 1 - 2 * instance.stateCount());
  }
}

export function changeAxis() {
  const xArray = axisArray($("#xAxis").prop("selectedIndex"));
  const yArray = axisArray($("#yAxis").prop("selectedIndex"));
  let dataSet = [];

  for (let i = 0; i < xArray.size(); ++i) {
    dataSet.push({ x: xArray.get(i), y: yArray.get(i) });
  }

  lineSeries.clear();
  lineSeries.add(dataSet);
  // lineSeries.addArraysXY(xArray, yArray);

  chart.getDefaultAxisX().fit();
  chart.getDefaultAxisY().fit();
}

function formattedIssueDescription(issue) {
  return issue.charAt(0).toLowerCase() + issue.slice(1);
}

// Make sure that the DOM is loaded before we do anything else.

import libOpenCOR from "../libopencor.js";

document.addEventListener("DOMContentLoaded", () => {
  // Make sure that libOpenCOR is loaded before we do anything else.

  libOpenCOR().then((libopencor) => {
    // Simulation page.

    const input = $("#dropAreaInput")[0];
    const dropArea = $("#dropArea")[0];
    let hasValidFile = false;

    input.addEventListener("change", (event) => {
      if (hasValidFile) {
        let inputFile = input.files[0];
        let fileReader = new FileReader();

        input.value = ""; // Allow the user to select the same file again.

        fileReader.readAsArrayBuffer(inputFile);

        fileReader.onload = async () => {
          try {
            // Retrieve the contents of the file.

            const fileArrayBuffer = await inputFile.arrayBuffer();
            const memPtr = libopencor._malloc(inputFile.size);
            const mem = new Uint8Array(
              libopencor.HEAPU8.buffer,
              memPtr,
              inputFile.size
            );

            mem.set(new Uint8Array(fileArrayBuffer));

            const file = new libopencor.File(inputFile.name);

            file.setContents(memPtr, inputFile.size);

            // Determine the type of the file.

            let fileType = "unknown";

            if (file.type() === libopencor.File.Type.CELLML_FILE) {
              fileType = "CellML";
            } else if (file.type() === libopencor.File.Type.SEDML_FILE) {
              fileType = "SED-ML";
            }

            $("#fileName").html(inputFile.name);
            $("#fileType").html(fileType);

            // Display any issues with the file or run it.

            let showIssues = false;

            if (file.type() === libopencor.File.Type.CELLML_FILE) {
              if (file.hasIssues()) {
                const issuesElement = $("#issues");
                const fileIssues = file.issues();

                issuesElement.empty();

                for (let i = 0; i < fileIssues.size(); ++i) {
                  const issue = fileIssues.get(i);

                  issuesElement.append(
                    '<li><span class="bold">' +
                      issue.typeAsString() +
                      ":</span>" +
                      formattedIssueDescription(issue.description()) +
                      "</li>"
                  );
                }

                showIssues = true;
              } else {
                // Retrieve some information about the simulation.

                sed = new libopencor.SedDocument(file);
                simulation = sed.simulations().get(0);

                if (simulation.constructor.name === "SedUniformTimeCourse") {
                  simulation.setOutputEndTime(50.0);
                  simulation.setNumberOfSteps(50000);
                  // simulation.setOutputEndTime(1.0);
                  // simulation.setNumberOfSteps(1000);
                }

                instance = sed.createInstance();

                // Populate the X and Y axis dropdown lists.

                populateAxis("xAxis");
                populateAxis("yAxis");

                // By default we plot the first state variable against the VOI.

                $("#xAxis").val(instance.voiName());
                $("#yAxis").val(instance.stateName(0));

                // Reset the plotting area.

                lineSeries.clear();

                // Run the model.

                run();
              }
            }

            updateFileUi(true, showIssues, false, true);
          } catch (exception) {
            showError(exception.message);
          }
        };

        fileReader.onerror = () => {
          showError(fileReader.error.message);
        };
      } else {
        resetFile();
      }
    });

    ["dragenter", "dragover"].forEach((eventName) => {
      dropArea.addEventListener(eventName, (event) => {
        if (event.dataTransfer.items.length === 1) {
          hasValidFile = true;

          dropArea.classList.add("drop-area-valid-file");
        } else {
          hasValidFile = false;

          dropArea.classList.add("drop-area-invalid-file-s");
        }
      });
    });

    ["dragleave", "drop"].forEach((eventName) => {
      dropArea.addEventListener(eventName, () => {
        dropArea.classList.remove("drop-area-valid-file");
        dropArea.classList.remove("drop-area-invalid-file-s");
      });
    });

    // Versions page.

    $("#version").html(libopencor.version());
    $("#versionString").html(libopencor.versionString());
    $("#libcellmlVersion").html(libopencor.libcellmlVersion());
    $("#libcellmlVersionString").html(libopencor.libcellmlVersionString());
    $("#libcombineVersion").html(libopencor.libcombineVersion());
    $("#libcombineVersionString").html(libopencor.libcombineVersionString());
    $("#libsedmlVersion").html(libopencor.libsedmlVersion());
    $("#libsedmlVersionString").html(libopencor.libsedmlVersionString());
    $("#sundialsVersion").html(libopencor.sundialsVersion());
    $("#sundialsVersionString").html(libopencor.sundialsVersionString());

    // Start with our simulation page.

    showPage("Simulation");
  });
});
