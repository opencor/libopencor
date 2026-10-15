As well as traditional tests, there is also a very simple website that makes use of libOpenCOR's WebAssembly.

After building libOpenCOR, you can run the website from the command line:

```bash
cd [libOpenCOR]
emrun --browser chrome build/tests/bindings/javascript/index.html
```

Note: libOpenCOR's JavaScript bindings require `SharedArrayBuffer` to run a simulation in a background thread (i.e. using `startRun()`), which in turn requires the page to be served with the `Cross-Origin-Opener-Policy: same-origin` and `Cross-Origin-Embedder-Policy: require-corp` HTTP headers. `emrun` serves them, but if you use another Web server, you must make sure that it serves them too (Web browsers ignore their `<meta>` equivalents). Otherwise, `startRun()` throws an error.
