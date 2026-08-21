# Updating

This directory contains a subset of [ImGui](https://github.com/ocornut/imgui), a bloat-free immediate mode GUI.

Note to our future selves: To update to a new ImGui version, copy the following files from the ImGui release to the new directory:

```sh
cp LICENSE.txt *.{cpp,h} backends/imgui_impl_glfw.{cpp,h} backends/imgui_impl_opengl3* external/imgui-X.XX.X
```

Also include `CMakeLists.txt` and `pmp_imconfig.h` from the current version.
