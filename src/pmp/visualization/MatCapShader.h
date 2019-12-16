//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

// clang-format off

static const char* matcap_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
    "\n"
    "layout (location=0) in vec4 v_position;\n"
    "layout (location=1) in vec3 v_normal;\n"
    "out vec2 v2f_texcoord;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "uniform mat4 modelview_matrix;\n"
    "uniform mat3 normal_matrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 n = normalize(normal_matrix * v_normal);\n"
    "   vec3 e = normalize(modelview_matrix * v_position).xyz;\n"
    "   vec3 r = reflect( e, n );\n"
    "   float m = 2.0 * sqrt( pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0) );\n"
    "   v2f_texcoord = r.xy / m + 0.5;\n"
    "   gl_Position = modelview_projection_matrix * v_position;\n"
    "}\n";


static const char* matcap_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
    "precision mediump float;\n"
#endif
    "\n"
    "in vec2 v2f_texcoord;\n"
    "uniform sampler2D matcap;\n"
    "uniform float  alpha;\n"
    "out vec4 f_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 rgb = texture(matcap, v2f_texcoord.xy).rgb;\n"
    "    rgb      = pow(clamp(rgb, 0.0, 1.0), vec3(0.45));\n"
    "    f_color  = vec4(rgb, alpha);\n"
    "}";


//=============================================================================
// clang-format on
//=============================================================================
