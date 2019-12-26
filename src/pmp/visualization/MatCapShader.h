//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

// clang-format off

// mat-cap shader: assume view=(0,0,-1), then the tex-coord for
// spherical environment mapping is just the normal's XY
// scaled by 0.5 and shifted by 0.5.
// scale by 0.49 to avoid artifacts at gracing angles
static const char* matcap_vshader = R"glsl(
#version 300 es

layout (location=0) in vec4 v_position;
layout (location=1) in vec3 v_normal;
out vec2 v2f_texcoord;
uniform mat4 modelview_projection_matrix;
uniform mat3 normal_matrix;

void main()
{
    vec3 n = normalize(normal_matrix * v_normal);
    v2f_texcoord = 0.49 * n.xy + 0.5;
    gl_Position = modelview_projection_matrix * v_position;
};
)glsl";


static const char* matcap_fshader = R"glsl(
#version 300 es
precision mediump float;

in vec2 v2f_texcoord;
uniform sampler2D matcap;
uniform float  alpha;
out vec4 f_color;

void main()
{
    vec4 rgba = texture(matcap, v2f_texcoord.xy);
    rgba.a *= alpha;
    f_color = rgba;
};
)glsl";


//=============================================================================
// clang-format on
//=============================================================================
