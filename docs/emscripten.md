# Building JavaScript Apps {#emscripten}

PMP supports seamless cross-compilation to JavaScript using [emscripten](https://github.com/kripken/emscripten). This allows you to build applications that run in a browser, just like we do with our mesh processing [demo application](https://www.pmp-library.org/mpview.html).

In order to build JavaScript applications, download the SDK from <https://github.com/kripken/emscripten> and follow the installation instructions.

Next, source the emscripten SDK environment setup script:

```sh
source <path_to_install_dir>/emsdk_env.sh
```

Create a separate build directory:

```sh
mkdir embuild
cd embuild
```

Run `cmake` through the `emcmake` wrapper and build:

```sh
emcmake cmake ..
make
```

Finally, start a local web server and open the corresponding app:

```sh
emrun mpview.html
```
