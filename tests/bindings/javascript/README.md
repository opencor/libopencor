As well as traditional tests, there is also a very simple website that makes use of libOpenCOR's WebAssembly.

After building libOpenCOR, you can run the website from the command line:

```bash
cd [libOpenCOR]
emrun --browser chrome build/tests/bindings/javascript/index.html
```

Note: libOpenCOR's JavaScript bindings require `SharedArrayBuffer` for pthread support, which in turn requires the `Cross-Origin-Opener-Policy: same-origin` and `Cross-Origin-Embedder-Policy: require-corp` HTTP headers (or their `<meta>` equivalents, which are already present in the HTML). If the demo page fails to load the WebAssembly module, verify that these headers are being served.
