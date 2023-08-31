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

  document.getElementById("fileError").innerHTML = error;

  updateFileUi("none", "block", "block");
}

function updateFileUi(fileInfoDisplay, fileErrorDisplay, resetButtonDisplay) {
  document.querySelector(".file-info").style.display = fileInfoDisplay;
  document.querySelector(".file-error").style.display = fileErrorDisplay;
  document.querySelector(".reset-button").style.display = resetButtonDisplay;
}

export function resetFile() {
  document.getElementById("dropAreaInput").value = "";

  updateFileUi("none", "none", "none");
}

// Make sure that the DOM is loaded before we do anything else.

import libOpenCOR from "../libopencor.js";

document.addEventListener("DOMContentLoaded", () => {
  // Make sure that libOpenCOR is loaded before we do anything else.

  libOpenCOR().then((libopencor) => {
    // Simulation page.

    const input = document.querySelector(".drop-area-input");
    const dropArea = document.querySelector(".drop-area");
    let hasValidFile = false;

    input.addEventListener("change", (event) => {
      if (hasValidFile) {
        let file = input.files[0];
        let fileReader = new FileReader();

        fileReader.readAsArrayBuffer(file);

        fileReader.onload = async () => {
          try {
            const fileArrayBuffer = await file.arrayBuffer();
            const memPtr = libopencor._malloc(file.size);
            const mem = new Uint8Array(
              libopencor.HEAPU8.buffer,
              memPtr,
              file.size,
            );

            mem.set(new Uint8Array(fileArrayBuffer));

            const libopencorFile = new libopencor.File(file.name);

            libopencorFile.setContents(memPtr, file.size);

            let libopencorFileType = "unknown file";

            switch (libopencorFile.type()) {
              case libopencor.File.Type.CELLML_FILE:
                libopencorFileType = "CellML file";

                break;
              case libopencor.File.Type.SEDML_FILE:
                libopencorFileType = "SED-ML file";

                break;
              default:
                break;
            }

            document.getElementById("fileName").innerHTML = file.name;
            document.getElementById("fileType").innerHTML = libopencorFileType;

            updateFileUi("block", "none", "block");
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

    document.getElementById("version").innerHTML = libopencor.version();
    document.getElementById("versionString").innerHTML =
      libopencor.versionString();
    document.getElementById("libcellmlVersion").innerHTML =
      libopencor.libcellmlVersion();
    document.getElementById("libcellmlVersionString").innerHTML =
      libopencor.libcellmlVersionString();
    document.getElementById("libcombineVersion").innerHTML =
      libopencor.libcombineVersion();
    document.getElementById("libcombineVersionString").innerHTML =
      libopencor.libcombineVersionString();
    document.getElementById("libsedmlVersion").innerHTML =
      libopencor.libsedmlVersion();
    document.getElementById("libsedmlVersionString").innerHTML =
      libopencor.libsedmlVersionString();
    document.getElementById("sundialsVersion").innerHTML =
      libopencor.sundialsVersion();
    document.getElementById("sundialsVersionString").innerHTML =
      libopencor.sundialsVersionString();

    // Show our home page.

    showPage("Home");
  });
});
