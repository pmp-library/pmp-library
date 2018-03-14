//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================
#ifndef __EMSCRIPTEN__ // standard version (OpenGL 3.2 and higher)

// clang-format off
static const char* phong_vshader =
    "#version 150\n"
    "\n"
    "in vec4 v_position;\n"
    "in vec3 v_normal;\n"
    "in vec2 v_tex;\n"
    "out vec3 v2f_normal;\n"
    "out vec2 v2f_tex;\n"
    "out vec3 v2f_view;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "uniform mat4 modelview_matrix;\n"
    "uniform mat3 normal_matrix;\n"
    "uniform bool show_texture_layout = false;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   v2f_normal  = normal_matrix * v_normal;\n"
    "   v2f_tex     = v_tex;\n"
    "   vec4 pos    = show_texture_layout ? vec4(v_tex, 0.0, 1.0) : v_position;\n"
    "   v2f_view    = -(modelview_matrix * pos).xyz;\n"
    "   gl_Position = modelview_projection_matrix * pos;\n"
    "} \n";


static const char* phong_fshader =
    "#version 150\n"
    "\n"
    "in vec3  v2f_normal;\n"
    "in vec2  v2f_tex;\n"
    "in vec3  v2f_view;\n"
    "\n"
    "uniform bool   use_lighting = true;\n"
    "uniform bool   use_texture  = false;\n"
    "uniform bool   use_srgb     = false;\n"
    "uniform vec3   front_color  = vec3(0.6, 0.6, 0.6);\n"
    "uniform vec3   back_color   = vec3(0.5, 0.0, 0.0);\n"
    "uniform float  ambient      = 0.1;\n"
    "uniform float  diffuse      = 0.8;\n"
    "uniform float  specular     = 0.6;\n"
    "uniform float  shininess    = 100.0;\n"
    "uniform vec3   light1       = vec3( 1.0, 1.0, 1.0);\n"
    "uniform vec3   light2       = vec3(-1.0, 1.0, 1.0);\n"
    "\n"
    "uniform sampler2D mytexture;\n"
    "\n"
    "out vec4 f_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 color = gl_FrontFacing ? front_color : back_color;\n"
    "    vec3 rgb;\n"
    "\n"
    "    if (use_lighting)\n"
    "    {\n"
    "       vec3 L1 = normalize(light1);\n"
    "       vec3 L2 = normalize(light2);\n"
    "       vec3 N  = normalize(v2f_normal);\n"
    "       vec3 V  = normalize(v2f_view);\n"
    "       \n"
    "       if (!gl_FrontFacing) N = -N;\n"
    "       \n"
    "       vec3  R;\n"
    "       float NL, RV;\n"
    "       \n"
    "       rgb = ambient * 0.1 * color;\n"
    "       \n"
    "       NL = dot(N, L1);\n"
    "       if (NL > 0.0)\n"
    "       {\n"
    "           rgb += diffuse * NL * color;\n"
    "           R  = normalize(-reflect(L1, N));\n"
    "           RV = dot(R, V);\n"
    "           if (RV > 0.0) \n"
    "           {\n"
    "               rgb += vec3( specular * pow(RV, shininess) );\n"
    "           }\n"
    "       }\n"
    "       \n"
    "       NL = dot(N, L2);\n"
    "       if (NL > 0.0)\n"
    "       {\n"
    "            rgb += diffuse * NL * color;\n"
    "            R  = normalize(-reflect(L2, N));\n"
    "            RV = dot(R, V);\n"
    "            if (RV > 0.0) \n"
    "            {\n"
    "                rgb += vec3( specular * pow(RV, shininess) );\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "   \n"
    "    // do not use lighting\n"
    "    else\n"
    "    {\n"
    "        rgb = color;\n"
    "    }\n"
    "    \n"
    "   if (use_texture) rgb *= texture(mytexture, v2f_tex).xyz;\n"
    "   if (use_srgb)    rgb  = pow(clamp(rgb, 0.0, 1.0), vec3(0.45));\n"
    "   \n"
    "    f_color = vec4(rgb, 1.0);\n"
    "}";


#else // emscripten WebGL-friendly version


static const char* phong_vshader =
    "attribute vec4  v_position;\n"
    "attribute vec3  v_normal;\n"
    "attribute vec2  v_tex;\n"
    "varying vec3  v2f_normal;\n"
    "varying vec3  v2f_view;\n"
    "varying vec2  v2f_tex;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "uniform mat4 modelview_matrix;\n"
    "uniform mat3 normal_matrix;\n"
    "uniform bool show_texture_layout;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_PointSize = 5.0;\n"
    "   v2f_normal  = normal_matrix * v_normal;\n"
    "   vec4 pos    = show_texture_layout ? vec4(v_tex, 0.0, 1.0) : v_position;\n"
    "   v2f_view    = -(modelview_matrix*pos).xyz;\n"
    "   v2f_tex     = v_tex;\n"
    "   gl_Position = modelview_projection_matrix * pos;\n"
    "}\n";

static const char* phong_fshader =
    "precision highp float;\n"
    "varying vec3   v2f_normal;\n"
    "varying vec3   v2f_view;\n"
    "varying vec2   v2f_tex;\n"
    "uniform bool   use_lighting;\n"
    "uniform bool   use_texture;\n"
    "uniform bool   use_srgb;\n"
    "uniform vec3   front_color;\n"
    "uniform vec3   back_color;\n"
    "uniform vec3   light1;\n"
    "uniform vec3   light2;\n"
    "uniform sampler2D mytexture;\n"
    "uniform float  ambient;\n"
    "uniform float  diffuse;\n"
    "uniform float  specular;\n"
    "uniform float  shininess;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 color = gl_FrontFacing ? front_color : back_color;\n"
    "    vec3 rgb;\n"
    "\n"
    "    if (use_lighting)\n"
    "    {\n"
    "        vec3 L1 = normalize(light1);\n"
    "    	 vec3 L2 = normalize(light2);\n"
    "        vec3 N  = normalize(v2f_normal);\n"
    "    	 vec3 V  = normalize(v2f_view);\n"
    "\n"
    "        if (!gl_FrontFacing) N = -N;\n"
    "\n"
    "        vec3  R;\n"
    "        float NL, RV;\n"
    "\n"
    "        rgb = ambient * 0.1 * color;\n"
    "\n"
    "        NL = dot(N, L1);\n"
    "        if (NL > 0.0)\n"
    "        {\n"
    "            rgb += diffuse * NL * color;\n"
    "            R  = normalize(-reflect(L1, N));\n"
    "            RV = dot(R, V);\n"
    "            if (RV > 0.0) \n"
    "            {\n"
    "                rgb += vec3( specular * pow(RV, shininess) );\n"
    "            }\n"
    "        }\n"
    "\n"
    "        NL = dot(N, L2);\n"
    "        if (NL > 0.0)\n"
    "        {\n"
    "            rgb += diffuse * NL * color;\n"
    "            R  = normalize(-reflect(L2, N));\n"
    "            RV = dot(R, V);\n"
    "            if (RV > 0.0) \n"
    "            {\n"
    "                rgb += vec3( specular * pow(RV, shininess) );\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "\n"
    "    // do not use lighting\n"
    "    else\n"
    "    {\n"
    "        rgb = color;\n"
    "    }\n"
    "   \n"
    "   if (use_texture) rgb *= texture2D(mytexture, v2f_tex).xyz;\n"
    "   if (use_srgb)    rgb  = pow(clamp(rgb, 0.0, 1.0), vec3(0.45));\n"
    "   \n"
    "   gl_FragColor = vec4(rgb, 1.0);\n"
    "}";

// clang-format on
#endif
