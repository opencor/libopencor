let loc = null;
let fileManager = null;
let file = null;
let document = null;
let simulation = null;
let instance = null;
let instanceTask = null;
let simulationId = 0;
let isPaused = false;
const { lightningChart } = lcjs;
const chart = lightningChart()
  .ChartXY({
    container: $('#plottingArea')[0]
  })
  .setTitle('')
  .setAnimationsEnabled(false);
const lineSeries = chart.addLineSeries().setName('');
let plottedPointCount = 0;
// Our delays (in microseconds), i.e. no delay, 1 to 10 µs in steps of 1 µs, 10 to 100 µs in steps of 10 µs, and 100 µs
// to 1 ms in steps of 100 µs, with a major step for 1 µs, 10 µs, 100 µs, and 1 ms (see setupDelayWheel()).
const DELAYS = [
  0,
  ...Array.from({ length: 10 }, (_, i) => i + 1),
  ...Array.from({ length: 9 }, (_, i) => 10 * (i + 2)),
  ...Array.from({ length: 9 }, (_, i) => 100 * (i + 2))
];
const DELAY_STEPS = DELAYS.length - 1;
const DELAY_MAJOR_STEPS = DELAYS.flatMap((delay, index) => ([1, 10, 100, 1000].includes(delay) ? [index] : []));
const DELAY_WHEEL_WIDTH = 120; // Must match the width of our wheel (see .delay-wheel in libopencor.css).
const DELAY_WHEEL_HEIGHT = 28; // Must match the height of our wheel (see .delay-wheel in libopencor.css).
const DELAY_WHEEL_BORDER_WIDTH = 1; // Must match the width of our border (see .delay-wheel in libopencor.css).
const DELAY_WHEEL_INNER_WIDTH = DELAY_WHEEL_WIDTH - 2 * DELAY_WHEEL_BORDER_WIDTH;
const DELAY_WHEEL_INNER_HEIGHT = DELAY_WHEEL_HEIGHT - 2 * DELAY_WHEEL_BORDER_WIDTH;
const DELAY_WHEEL_RADIUS = 0.5 * DELAY_WHEEL_INNER_WIDTH;
const DELAY_WHEEL_TICK_SPACING = 10; // Spacing between two ticks, in degrees.
const DELAY_WHEEL_POSITION_AREA_HEIGHT = 7; // Height of our position bar and of the space below it.
const DELAY_WHEEL_MARKER_GAP = 2; // Space between our marker and our position bar.
const DELAY_WHEEL_MARKER_WIDTH = 2;
const DELAY_WHEEL_MARKER_TOP = 2;
const DELAY_WHEEL_MARKER_BOTTOM = DELAY_WHEEL_INNER_HEIGHT - DELAY_WHEEL_POSITION_AREA_HEIGHT - DELAY_WHEEL_MARKER_GAP;
const DELAY_WHEEL_TICKS_MIDDLE = 0.5 * (DELAY_WHEEL_MARKER_TOP + DELAY_WHEEL_MARKER_BOTTOM);
const DELAY_WHEEL_END_TICK_WIDTH = 2; // Must match the stroke width of our stop ticks (see libopencor.css).
const DELAY_WHEEL_END_TICK_HALF_LENGTH =
  0.5 * (DELAY_WHEEL_MARKER_BOTTOM - DELAY_WHEEL_MARKER_TOP - DELAY_WHEEL_END_TICK_WIDTH) - 0.5;
const DELAY_WHEEL_MAJOR_TICK_HALF_LENGTH = DELAY_WHEEL_END_TICK_HALF_LENGTH - 1.5;
const DELAY_WHEEL_MINOR_TICK_HALF_LENGTH = DELAY_WHEEL_END_TICK_HALF_LENGTH - 3.5;
const DELAY_WHEEL_TURN_DURATION = 120; // How long our wheel takes to turn to its new step (in milliseconds).
const DELAY_WHEEL_DRAG_RANGE = (DELAY_STEPS * DELAY_WHEEL_RADIUS * DELAY_WHEEL_TICK_SPACING * Math.PI) / 180;
const DELAY_WHEEL_SCROLL_RANGE = 1500; // How much our wheel must be scrolled (in pixels) to go from one end to the other.
const DELAY_WHEEL_LINE_HEIGHT = 16; // Height of a line (in pixels), for scrolls that are expressed in lines.
const DELAY_WHEEL_PAGE_KEY_STEPS = Math.max(1, Math.round(0.1 * DELAY_STEPS));
let delayStep = 0;
let delayRawStep = 0; // Unsnapped step, so that small drags and scrolls eventually move our wheel by a step.
let delayDisplayedStep = 0; // Step shown by our wheel, which follows our step smoothly.
let delayTurnAnimationFrame = 0;

export function showPage(page) {
  $('.nav-link').each(function () {
    if (this.id === `nav${page}`) {
      this.classList.add('active');
    } else {
      this.classList.remove('active');
    }
  });

  $('.page').each(function () {
    this.style.display = this.id === `page${page}` ? 'block' : 'none';
  });
}

function showIssues(issues) {
  const issuesElement = $('#issues');

  issuesElement.empty();

  if (Array.isArray(issues)) {
    for (const issue of issues) {
      issuesElement.append(`<li><span·class="bold">Error:</span>·${formattedIssueDescription(issue)}</li>`);
    }
  } else {
    for (const issue of issues) {
      issuesElement.append(
        '<li><span class="bold">' +
          issue.typeAsString +
          ':</span> ' +
          formattedIssueDescription(issue.description) +
          '</li>'
      );
    }
  }

  updateFileUi(true, true, true, false);
}

function listFiles() {
  console.log('-------------------');

  if (fileManager.hasFiles) {
    console.log('Files:');

    const files = fileManager.files;

    for (const file of files) {
      console.log(` - ${file.fileName}`);
    }
  } else {
    console.log('No files.');
  }
}

function updateFileUi(fileInfoDisplay, fileIssuesDisplay, resetButtonDisplay, simulationDisplay) {
  $('#fileInfo').css('display', fileInfoDisplay ? 'block' : 'none');
  $('#fileIssues').css('display', fileIssuesDisplay ? 'block' : 'none');
  $('#reset').css('display', resetButtonDisplay ? 'block' : 'none');
  $('#simulation').css('display', simulationDisplay ? 'block' : 'none');

  listFiles();
}

export function reset() {
  fileManager.reset();

  updateFileUi(false, false, false, false);
}

function addAxisElement(axis, name) {
  axis.append(`<option>${name}</option>`);
}

function populateAxis(axisId) {
  const axis = $(`#${axisId}`);

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

  for (let i = 0; i < instanceTask.algebraicVariableCount; ++i) {
    addAxisElement(axis, instanceTask.algebraicVariableName(i));
  }
}

export function run() {
  if (instance.status !== loc.SedInstance.Status.IDLE) {
    // Toggle between pause and resume.

    if (isPaused) {
      isPaused = false;

      $('#run').text('Pause');

      instance.resumeRun();
    } else {
      isPaused = true;

      $('#run').text('Resume');

      instance.pauseRun();
    }

    return;
  }

  // Keep track of which simulation this is so we can ignore stale callbacks.

  const currentSimulationId = ++simulationId;

  // Disable the run button.

  $('#run').prop('disabled', true);

  // Reset the plotting area and the progress bar.

  lineSeries.clear();

  plottedPointCount = 0;

  const $progressBarFill = $('#progressBarFill');

  $progressBarFill.css('width', '0%');
  $progressBarFill.removeClass('complete');

  // Retrieve the duration of the simulation and the number of steps.

  simulation.outputEndTime = $('#simulationDuration').val();
  simulation.numberOfSteps = $('#simulationDuration').val() / $('#simulationInterval').val();

  // Run the simulation either synchronously or asynchronously.

  setTimeout(() => {
    if (simulationId !== currentSimulationId) {
      return;
    }

    if ($('#runAsync').is(':checked')) {
      runAsync();
    } else {
      runSync();
    }
  }, 0);
}

export function stopSimulation() {
  instance.stopRun();

  isPaused = false;

  $('#run').text('Run').prop('disabled', true);
  $('#stop').prop('disabled', true);
}

function plotAndUpdateProgressBar() {
  const currentSimulationId = simulationId;
  const $progressBarFill = $('#progressBarFill');

  $progressBarFill.css('width', `${instance.progress * 100}%`);

  if (instance.progress >= 1.0) {
    $progressBarFill.addClass('complete');
  }

  setTimeout(() => {
    if (simulationId !== currentSimulationId) {
      return;
    }

    // Update the plotting area and the axes information.

    console.time('Plotting time');
    updatePlottingAreaAndAxesInfo();
    console.timeEnd('Plotting time');

    // Reset the progress bar after a short delay.

    setTimeout(() => {
      $progressBarFill.css('width', '0%');
      $progressBarFill.removeClass('complete');
    }, 169);
  }, 0);
}

function runSync() {
  applyDelay();

  console.log('-------------------');
  console.time('Computing time');
  instance.run();
  console.timeEnd('Computing time');

  $('#run').prop('disabled', false);

  plotAndUpdateProgressBar();
}

function runAsync() {
  const currentSimulationId = simulationId;
  const $progressBarFill = $('#progressBarFill');

  applyDelay();

  console.log('-------------------');
  console.time('Computing time');
  instance.startRun();

  // Update the button states for a running simulation.

  $('#run').text('Pause').prop('disabled', false);
  $('#stop').prop('disabled', false);

  const intervalId = setInterval(() => {
    if (simulationId !== currentSimulationId) {
      clearInterval(intervalId);

      return;
    }

    const progress = instance.progress;

    $progressBarFill.css('width', `${progress * 100}%`);

    if (progress >= 1.0) {
      $progressBarFill.addClass('complete');
    }

    // Update the plot with the data computed so far.

    const completedSteps = Math.round(progress * simulation.numberOfSteps);

    if (completedSteps > 0) {
      updatePlottingAreaAndAxesInfo(completedSteps + 1);
    }

    if (instance.status === loc.SedInstance.Status.IDLE) {
      clearInterval(intervalId);
      instance.waitForRun();
      console.timeEnd('Computing time');

      // Reset the button states.

      isPaused = false;

      $('#run').text('Run').prop('disabled', false);
      $('#stop').prop('disabled', true);

      plotAndUpdateProgressBar();
    }
  }, 50);
}

function isAsyncRun() {
  return $('#runAsync').is(':checked');
}

function delay() {
  // Note: our delays are in microseconds, but libOpenCOR expects them in milliseconds.

  return DELAYS[delayStep] / 1000;
}

function formattedNumber(number) {
  // Format the given number using three significant digits, but without any decimals if they are all zeroes (e.g.,
  // "1.00" becomes "1" while "1.50" remains "1.50").

  return number.toPrecision(3).replace(/\.0+$/, '');
}

function formattedTime(time) {
  // Format the given time, which is in milliseconds, using the most appropriate unit.
  // Note: we round our time before choosing our unit so that, for instance, 0.9999 ms is formatted as "1 ms" rather
  //       than as "1.00e+3 µs".

  const roundedTime = Number(time.toPrecision(3));

  if (roundedTime < 1) {
    return `${formattedNumber(1000 * roundedTime)} µs`;
  }

  if (roundedTime < 1000) {
    return `${formattedNumber(roundedTime)} ms`;
  }

  if (roundedTime < 60000) {
    return `${formattedNumber(roundedTime / 1000)} s`;
  }

  return `${formattedNumber(roundedTime / 60000)} min`;
}

function simulationStepCount() {
  const stepCount = $('#simulationDuration').val() / $('#simulationInterval').val();

  return Number.isFinite(stepCount) ? Math.round(stepCount) : 0;
}

export function updateDelayInfo() {
  // Let the user know how long our delay would, at least, make a run take.

  const value = delay();

  $('#delayValue').text(value === 0 ? 'No delay' : formattedTime(value));
  $('#delayTotal').text(value === 0 ? '' : `(≥ ${formattedTime(value * simulationStepCount())} per run)`);
}

function applyDelay() {
  // Note: a synchronous run blocks the page for as long as it runs, so we only apply our delay to asynchronous runs.

  if (instance !== null) {
    instance.delay = isAsyncRun() ? delay() : 0;
  }
}

function drawDelayWheel() {
  // Draw our wheel as a ruler dial, i.e. as a scale on a cylinder seen from above (so its ticks get closer to one
  // another towards the sides of our wheel, where they also fade out, see index.html), with a tick for each of our
  // steps, a longer tick for each of our major steps, and a stop tick at each end of our scale. Only the ticks on the
  // visible half of our cylinder are drawn and nothing is drawn beyond our stop ticks, so one side of our wheel is
  // empty when we are at the start or end of our wheel.

  const rotation = delayDisplayedStep * DELAY_WHEEL_TICK_SPACING;
  const firstStep = Math.max(0, Math.ceil((rotation - 90) / DELAY_WHEEL_TICK_SPACING));
  const lastStep = Math.min(DELAY_STEPS, Math.floor((rotation + 90) / DELAY_WHEEL_TICK_SPACING));
  let ticks = '';

  for (let step = firstStep; step <= lastStep; ++step) {
    const angle = step * DELAY_WHEEL_TICK_SPACING - rotation;
    const x = DELAY_WHEEL_RADIUS * (1 + Math.sin((angle * Math.PI) / 180));
    const end = step === 0 || step === DELAY_STEPS;
    const major = DELAY_MAJOR_STEPS.includes(step);
    const halfLength = end
      ? DELAY_WHEEL_END_TICK_HALF_LENGTH
      : major
        ? DELAY_WHEEL_MAJOR_TICK_HALF_LENGTH
        : DELAY_WHEEL_MINOR_TICK_HALF_LENGTH;
    const tickClass = end ? 'delay-wheel-tick-end' : major ? 'delay-wheel-tick-major' : '';

    ticks += `<line class="${tickClass}" x1="${x}" y1="${DELAY_WHEEL_TICKS_MIDDLE - halfLength}" x2="${x}" y2="${DELAY_WHEEL_TICKS_MIDDLE + halfLength}" />`;
  }

  $('#delayWheelTicks').html(ticks);
  $('#delayWheelPositionFill').css('width', `${(100 * delayDisplayedStep) / DELAY_STEPS}%`);
}

function turnDelayWheel() {
  // Turn our wheel smoothly to its new step, unless reduced motion is requested, in which case it gets there
  // straightaway.

  cancelAnimationFrame(delayTurnAnimationFrame);

  if (window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
    delayDisplayedStep = delayStep;

    drawDelayWheel();

    return;
  }

  const startStep = delayDisplayedStep;
  const startTime = performance.now();
  const turn = (time) => {
    const progress = Math.min((time - startTime) / DELAY_WHEEL_TURN_DURATION, 1);
    const easedProgress = 1 - (1 - progress) ** 3; // Ease out.

    delayDisplayedStep = startStep + (delayStep - startStep) * easedProgress;

    drawDelayWheel();

    if (progress < 1) {
      delayTurnAnimationFrame = requestAnimationFrame(turn);
    }
  };

  delayTurnAnimationFrame = requestAnimationFrame(turn);
}

function setDelayStep(step) {
  // Set our (unsnapped) step and, if our (snapped) step has changed, turn our wheel to it and apply our new delay
  // straight away, so that it can be changed while a simulation is running.

  delayRawStep = Math.min(Math.max(step, 0), DELAY_STEPS);

  const newDelayStep = Math.round(delayRawStep);

  if (newDelayStep !== delayStep) {
    delayStep = newDelayStep;

    turnDelayWheel();
    updateDelayInfo();
    applyDelay();
  }
}

export function setupDelayWheel() {
  // Set up our wheel (a bit like a thumbwheel or a jog wheel), which has a marker in its middle and a position bar at
  // its bottom.
  // Note: our ticks and our marker are drawn within our border.

  const $wheel = $('#delayWheel');
  const wheel = $wheel[0];

  $('#delayWheelSvg').attr('viewBox', `0 0 ${DELAY_WHEEL_INNER_WIDTH} ${DELAY_WHEEL_INNER_HEIGHT}`);
  $('#delayWheelFade').attr('x2', DELAY_WHEEL_INNER_WIDTH);
  $('#delayWheelMask, #delayWheelMaskRect').attr({ width: DELAY_WHEEL_INNER_WIDTH, height: DELAY_WHEEL_INNER_HEIGHT });
  $('#delayWheelMarker').attr({
    x: DELAY_WHEEL_RADIUS - 0.5 * DELAY_WHEEL_MARKER_WIDTH,
    y: DELAY_WHEEL_MARKER_TOP,
    width: DELAY_WHEEL_MARKER_WIDTH,
    height: DELAY_WHEEL_MARKER_BOTTOM - DELAY_WHEEL_MARKER_TOP,
    rx: 0.5 * DELAY_WHEEL_MARKER_WIDTH
  });

  // Drag our wheel left/right to increase/decrease our delay, i.e. our scale follows the mouse pointer, like the
  // surface of a physical wheel, so dragging our wheel left brings the ticks on the right (i.e. longer delays) under
  // our marker.

  let dragStartX = null;
  let dragStartStep = 0;

  wheel.addEventListener('pointerdown', (event) => {
    if (!isAsyncRun() || event.button !== 0) {
      return;
    }

    dragStartX = event.clientX;
    dragStartStep = delayStep;

    $wheel.addClass('dragging');

    wheel.setPointerCapture(event.pointerId);
    wheel.focus();
  });

  wheel.addEventListener('pointermove', (event) => {
    if (dragStartX !== null) {
      setDelayStep(dragStartStep - (DELAY_STEPS * (event.clientX - dragStartX)) / DELAY_WHEEL_DRAG_RANGE);
    }
  });

  ['pointerup', 'pointercancel', 'lostpointercapture'].forEach((eventName) => {
    wheel.addEventListener(eventName, () => {
      dragStartX = null;

      $wheel.removeClass('dragging');
    });
  });

  // Scroll our wheel (sideways or up/down) to increase/decrease our delay.
  // Note: a scroll may be expressed in lines (e.g., in Firefox) rather than in pixels, hence we convert it to pixels.

  wheel.addEventListener(
    'wheel',
    (event) => {
      if (!isAsyncRun()) {
        return;
      }

      event.preventDefault();

      const delta = Math.abs(event.deltaX) > Math.abs(event.deltaY) ? event.deltaX : -event.deltaY;
      const pixelDelta = event.deltaMode === WheelEvent.DOM_DELTA_LINE ? DELAY_WHEEL_LINE_HEIGHT * delta : delta;

      setDelayStep(delayRawStep + (DELAY_STEPS * pixelDelta) / DELAY_WHEEL_SCROLL_RANGE);
    },
    { passive: false }
  );

  // Use the arrow keys to change our delay by one step, the Page Up/Down keys to change it by several steps, and the
  // Home and End keys to go to the start and end of our wheel.

  wheel.addEventListener('keydown', (event) => {
    if (!isAsyncRun()) {
      return;
    }

    const steps = {
      ArrowUp: delayStep + 1,
      ArrowRight: delayStep + 1,
      ArrowDown: delayStep - 1,
      ArrowLeft: delayStep - 1,
      PageUp: delayStep + DELAY_WHEEL_PAGE_KEY_STEPS,
      PageDown: delayStep - DELAY_WHEEL_PAGE_KEY_STEPS,
      Home: 0,
      End: DELAY_STEPS
    };

    if (event.key in steps) {
      event.preventDefault();

      setDelayStep(steps[event.key]);
    }
  });

  // Double-click our wheel to reset our delay.

  wheel.addEventListener('dblclick', () => {
    if (isAsyncRun()) {
      setDelayStep(0);
    }
  });

  // Our delay only applies to asynchronous runs (see applyDelay()), so disable our wheel for synchronous ones.

  $('#runAsync').on('change', () => {
    $wheel.toggleClass('disabled', !isAsyncRun());
    $wheel.attr('tabindex', isAsyncRun() ? 0 : -1);

    applyDelay();
  });

  drawDelayWheel();
  updateDelayInfo();
}

function axisInfo(index) {
  // Retrieve and return the array and unit corresponding to the given index.

  if (index === 0) {
    return [instanceTask.voi, instanceTask.voiUnit];
  }

  index -= 1;

  if (index < 2 * instanceTask.stateCount) {
    const stateOrRateIndex = Math.floor(index / 2);

    if (index % 2 === 0) {
      return [instanceTask.state(stateOrRateIndex), instanceTask.stateUnit(stateOrRateIndex)];
    }

    return [instanceTask.rate(stateOrRateIndex), instanceTask.rateUnit(stateOrRateIndex)];
  }

  index -= 2 * instanceTask.stateCount;

  if (index < instanceTask.constantCount) {
    return [instanceTask.constant(index), instanceTask.constantUnit(index)];
  }

  index -= instanceTask.constantCount;

  if (index < instanceTask.computedConstantCount) {
    return [instanceTask.computedConstant(index), instanceTask.computedConstantUnit(index)];
  }

  index -= instanceTask.computedConstantCount;

  return [instanceTask.algebraicVariable(index), instanceTask.algebraicVariableUnit(index)];
}

export function updateAxisIntervals() {
  const xAxisIndex = $('#xAxis').prop('selectedIndex');
  const yAxisIndex = $('#yAxis').prop('selectedIndex');

  if (xAxisIndex === 0) {
    chart.getDefaultAxisX().setInterval({ start: 0, end: parseFloat($('#simulationDuration').val()) });
  } else {
    chart.getDefaultAxisX().fit();
  }

  if (yAxisIndex === 0) {
    chart.getDefaultAxisY().setInterval({ start: 0, end: parseFloat($('#simulationDuration').val()) });
  } else {
    chart.getDefaultAxisY().fit();
  }
}

export function updatePlottingAreaAndAxesInfo(pointCount = 0) {
  const xAxisIndex = $('#xAxis').prop('selectedIndex');
  const yAxisIndex = $('#yAxis').prop('selectedIndex');
  const [xAxisArray, xAxisUnit] = axisInfo(xAxisIndex);
  const [yAxisArray, yAxisUnit] = axisInfo(yAxisIndex);

  if (pointCount > 0) {
    const newCount = Math.min(pointCount, xAxisArray.length);

    if (newCount > plottedPointCount) {
      lineSeries.addArraysXY(
        xAxisArray.slice(plottedPointCount, newCount),
        yAxisArray.slice(plottedPointCount, newCount)
      );

      plottedPointCount = newCount;
    }
  } else {
    lineSeries.clear();

    lineSeries.addArraysXY(xAxisArray, yAxisArray);

    plottedPointCount = xAxisArray.length;
  }

  $('#xAxisUnit').text(xAxisUnit);
  $('#yAxisUnit').text(yAxisUnit);
}

function formattedIssueDescription(issue) {
  if (issue.startsWith('CellML') || issue.startsWith('SED-ML') || issue.startsWith('COMBINE')) {
    return issue;
  }

  return issue.charAt(0).toLowerCase() + issue.slice(1);
}

// Make sure that the DOM is loaded before we do anything else.

import libOpenCOR from '../libopencor.js';

$(() => {
  // Make sure that libOpenCOR is loaded before we do anything else.

  libOpenCOR().then((result) => {
    // Keep track of libOpenCOR.

    loc = result;

    // Keep track of the library and the file manager.

    fileManager = loc.FileManager.instance();

    // Simulation page.

    const input = $('#dropAreaInput')[0];
    const dropArea = $('#dropArea');
    let hasValidFile = false;

    input.onchange = () => {
      if (hasValidFile) {
        const inputFile = input.files[0];
        const fileReader = new FileReader();

        fileReader.onload = () => {
          try {
            // Retrieve the contents of the file and pass a Uint8Array to the binding.

            file = new loc.File(inputFile.name);

            file.setContents(new Uint8Array(fileReader.result));

            // Determine the type of the file.

            let fileType = 'some unknown file';
            let knownFile = true;

            if (file.type === loc.File.Type.CELLML_FILE) {
              fileType = 'a CellML file';
            } else if (file.type === loc.File.Type.SEDML_FILE) {
              fileType = 'a SED-ML file';
            } else if (file.type === loc.File.Type.COMBINE_ARCHIVE) {
              fileType = 'a COMBINE archive';
            } else {
              knownFile = false;
            }

            $('#fileName').html(inputFile.name);
            $('#fileType').html(fileType);

            // Display any issues with the file or run it.

            if (knownFile) {
              if (file.hasIssues) {
                showIssues(file.issues);
              } else {
                // Retrieve some information about the simulation.

                document = new loc.SedDocument(file);

                if (document.hasIssues) {
                  showIssues(document.issues);

                  return;
                }

                simulation = document.simulations[0];

                if (simulation?.hasIssues) {
                  showIssues(simulation.issues);

                  return;
                }

                instance = document.instantiate();

                if (instance.hasIssues) {
                  showIssues(instance.issues);

                  return;
                }

                instanceTask = instance.tasks[0];

                applyDelay();

                if (instanceTask?.hasIssues) {
                  showIssues(instanceTask.issues);

                  return;
                }

                $('#simulationDuration').val(simulation.outputEndTime);
                $('#simulationDurationUnit').text(instanceTask.voiUnit);

                $('#simulationInterval').val(simulation.outputEndTime / simulation.numberOfSteps);
                $('#simulationIntervalUnit').text(instanceTask.voiUnit);

                updateDelayInfo();

                // Populate the X and Y axis dropdown lists.

                populateAxis('xAxis');
                populateAxis('yAxis');

                // By default we plot the first state variable against the VOI.

                $('#xAxis').val(instanceTask.voiName);
                $('#yAxis').val(instanceTask.stateName(0));

                // Update the plotting area and the axes information.

                updatePlottingAreaAndAxesInfo();
                updateAxisIntervals();

                updateFileUi(true, false, true, true);
              }
            }
          } catch (exception) {
            showIssues([exception.message]);
          }
        };

        fileReader.onerror = () => {
          showIssues([fileReader.error.message]);
        };

        // Reset the input so that the user can select the same file again if they want to.

        input.value = '';

        // Read the file as an array buffer.

        fileReader.readAsArrayBuffer(inputFile);
      } else {
        updateFileUi(false, false, false, false);
      }
    };

    ['dragenter', 'dragover'].forEach((eventName) => {
      dropArea[0].addEventListener(eventName, (event) => {
        if (event.dataTransfer.items.length === 1) {
          hasValidFile = true;

          dropArea.addClass('drop-area-valid-file');
        } else {
          hasValidFile = false;

          dropArea.addClass('drop-area-invalid-file-s');
        }
      });
    });

    ['dragleave', 'drop'].forEach((eventName) => {
      dropArea.on(eventName, () => {
        dropArea.removeClass('drop-area-valid-file');
        dropArea.removeClass('drop-area-invalid-file-s');
      });
    });

    // Versions page.

    $('#libCellML').html(loc.libcellmlVersionString());
    $('#libCOMBINE').html(loc.libcombineVersionString());
    $('#libOpenCOR').html(loc.versionString());
    $('#libSEDML').html(loc.libsedmlVersionString());
    $('#LLVM').html(loc.llvmVersionString());
    $('#SUNDIALS').html(loc.sundialsVersionString());

    // Start with our simulation page.

    showPage('Simulation');
  });
});
