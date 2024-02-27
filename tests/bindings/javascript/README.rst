As well as traditional tests, there is also a very simple website that makes use of libOpenCOR's WebAssembly.

After building libOpenCOR, you can run the website from the command line:

.. code-block:: bash

    cd [libOpenCOR]/build
    emrun --browser chrome tests/bindings/javascript/index.html
