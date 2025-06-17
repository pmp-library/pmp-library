// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

// clang-format off

// mat-cap shader: assume view=(0,0,-1), then the tex-coord for
// spherical environment mapping is just the normal's XY
// scaled by 0.5 and shifted by 0.5.
// scale by 0.49 to avoid artifacts at gracing angles
static const char* matcap_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330"
#else
    "#version 300 es"
#endif
R"glsl(
layout (location=0) in vec4 v_position;
layout (location=1) in vec3 v_normal;
layout (location=4) in float v_selection;

out vec3 v2f_normal;
out float v2f_selection;

uniform mat4 modelview_projection_matrix;
uniform mat3 normal_matrix;

void main()
{
    v2f_normal = normalize(normal_matrix * v_normal);
    v2f_selection = v_selection;
    gl_Position = modelview_projection_matrix * v_position;
}
)glsl";


static const char* matcap_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330"
#else
    "#version 300 es"
#endif
R"glsl(
precision mediump float;

in vec3 v2f_normal;
in float v2f_selection;

uniform sampler2D matcap;
uniform float alpha;
uniform bool use_vertex_selection;

out vec4 f_color;

vec2 uv;
vec4 rgba;

void main()
{
    if (gl_FrontFacing)
    {
        uv = normalize(v2f_normal).xy * 0.49 + 0.5;
        rgba = texture(matcap, uv);
    }
    else
    {
        // invert normal, damp color
        uv = normalize(-v2f_normal).xy * 0.49 + 0.5;
        rgba = texture(matcap, uv);
        rgba.rgb *= 0.5;
    }

    if (use_vertex_selection)
        rgba.rgb *= v2f_selection > 0.5 ? vec3(0.7, 1.0, 0.7) : vec3(1.0, 0.7, 0.7);

    rgba.rgb  = pow(clamp(rgba.rgb, 0.0, 1.0), vec3(0.45));

    rgba.a *= alpha;
    f_color = rgba;
}
)glsl";

// clang-format on
