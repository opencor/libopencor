import libOpenCOR from "../libopencor.js";

libOpenCOR().then(function (libopencor) {
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
});

export function showPage(page) {
  document.querySelectorAll(".nav-link").forEach((crtPage) => {
    if (crtPage.id === "nav-" + page) {
      crtPage.classList.add("active");
    } else {
      crtPage.classList.remove("active");
    }
  });

  document.querySelectorAll(".page").forEach((crtPage) => {
    crtPage.style.display = crtPage.id === page + "-page" ? "block" : "none";
  });
}
