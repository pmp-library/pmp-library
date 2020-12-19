// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

// clang-format off

static const char* phong_vshader =
#ifndef __EMSCRIPTEN__
    "#version 330"
#else
    "#version 300 es"
#endif
R"glsl(
layout (location=0) in vec4 v_position;
layout (location=1) in vec3 v_normal;
layout (location=2) in vec2 v_tex;
layout (location=3) in vec3 v_color;

out vec3 v2f_normal;
out vec2 v2f_tex;
out vec3 v2f_view;
out vec3 v2f_color;

uniform mat4 modelview_projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform float point_size;
uniform bool show_texture_layout;

void main()
{
    v2f_normal   = normal_matrix * v_normal;
    v2f_tex      = v_tex;
    vec4 pos     = show_texture_layout ? vec4(v_tex, 0.0, 1.0) : v_position;
    v2f_view     = -(modelview_matrix * pos).xyz;
    v2f_color    = v_color;
    gl_PointSize = point_size;
    gl_Position  = modelview_projection_matrix * pos;
}
)glsl";


static const char* phong_fshader =
#ifndef __EMSCRIPTEN__
    "#version 330\n"
#else
    "#version 300 es\n"
#endif
R"glsl(
precision mediump float;

in vec3  v2f_normal;
in vec2  v2f_tex;
in vec3  v2f_view;
in vec3  v2f_color;

uniform bool   use_lighting;
uniform bool   use_texture;
uniform bool   use_srgb;
uniform bool   use_vertex_color;
uniform vec3   front_color;
uniform vec3   back_color;
uniform float  ambient;
uniform float  diffuse;
uniform float  specular;
uniform float  shininess;
uniform float  alpha;
uniform vec3   light1;
uniform vec3   light2;

uniform sampler2D mytexture;

out vec4 f_color;

void main()
{
    vec3 color = use_vertex_color ? v2f_color : (gl_FrontFacing ? front_color : back_color);

    vec3 rgb;

    if (use_lighting)
    {
        vec3 L1 = normalize(light1);
        vec3 L2 = normalize(light2);
        vec3 V  = normalize(v2f_view);
        vec3 N  = gl_FrontFacing ? normalize(v2f_normal) : -normalize(v2f_normal);
        vec3 R;
        float NL, RV;

        rgb = ambient * 0.1 * color;

        NL = dot(N, L1);
        if (NL > 0.0)
        {
            rgb += diffuse * NL * color;
            R  = normalize(-reflect(L1, N));
            RV = dot(R, V);
            if (RV > 0.0)
            {
                rgb += vec3( specular * pow(RV, shininess) );
            }
        }

        NL = dot(N, L2);
        if (NL > 0.0)
        {
            rgb += diffuse * NL * color;
            R  = normalize(-reflect(L2, N));
            RV = dot(R, V);
            if (RV > 0.0)
            {
                rgb += vec3( specular * pow(RV, shininess) );
            }
        }
    }

    // do not use lighting
    else
    {
        rgb = color;
    }

    if (use_texture) rgb *= texture(mytexture, v2f_tex).xyz;
    if (use_srgb)    rgb  = pow(clamp(rgb, 0.0, 1.0), vec3(0.45));

    f_color = vec4(rgb, alpha);
}
)glsl";

// clang-format on
