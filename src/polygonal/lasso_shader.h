// Copyright 2025 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

// clang-format off

static const char* lasso_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330"
#else
    "#version 300 es"
#endif
R"glsl(
layout(location = 0) in vec2 v_position; // in NDC: [-1, 1]
void main() {
    gl_Position = vec4(v_position, 0.0, 1.0);
}
)glsl";


static const char* lasso_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
R"glsl(
precision mediump float;
out vec4 fragColor;
uniform vec3 color;
void main() {
    fragColor = vec4(color, 1);
}
)glsl";

// clang-format on
