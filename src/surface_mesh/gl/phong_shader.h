
#ifndef __EMSCRIPTEN__  // standard version (OpenGL 3.2 and higher)


static const char* phong_vshader =
    "#version 150\n"
    "\n"
    "in vec4 v_position;\n"
    "in vec3 v_normal;\n"
    "out vec3 v2f_normal;\n"
    "out vec3 v2f_view;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "uniform mat4 modelview_matrix;\n"
    "uniform mat3 normal_matrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   v2f_normal  = normal_matrix * v_normal;\n"
    "   v2f_view    = -(modelview_matrix*v_position).xyz;\n"
    "   gl_Position = modelview_projection_matrix * v_position;\n"
    "} \n";


static const char* phong_fshader = 
    "#version 150\n"
    "\n"
    "in vec3 v2f_normal;\n"
    "in vec3 v2f_view;\n"
    "uniform vec3 light1;\n"
    "uniform vec3 light2;\n"
    "uniform vec3 color;\n"
    "out vec4 f_color;\n"
    "float p=100.0;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 L1 = normalize(light1);\n"
    "   vec3 L2 = normalize(light2);\n"
    "   vec3 N  = normalize(v2f_normal);\n"
    "   vec3 V  = normalize(v2f_view);\n"
    "   vec3 Rv = reflect(-V,N);\n"
    "   \n"
    "   float ambient  = 0.1;\n"
    "   float diffuse  = abs(dot(N,L1)) + abs(dot(N,L2));\n"
    "   float specular = max(0, pow(dot(Rv,L1),p)) + max(0, pow(dot(Rv,L2),p));\n"
    "   \n"
    "   vec3  rgb = color * (ambient + diffuse) + vec3(1,1,1) * specular;\n"
    "   \n"
    "   f_color = vec4(rgb, 1.0);\n"
    "}\n";


#else // emscripten WebGL-friendly version


static const char* phong_vshader = 
    "attribute vec4 v_position;\n"
    "attribute vec3 v_normal;\n"
    "varying vec3 v2f_normal;\n"
    "varying vec3 v2f_view;\n"
    "uniform mat4 modelview_projection_matrix;\n"
    "uniform mat4 modelview_matrix;\n"
    "uniform mat3 normal_matrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_PointSize = 5.0;\n"
    "   v2f_normal  = normal_matrix * v_normal;\n"
    "   v2f_view    = -(modelview_matrix*v_position).xyz;\n"
    "   gl_Position = modelview_projection_matrix * v_position;\n"
    "}\n";

static const char* phong_fshader = 
    "precision highp float;\n"
    "varying vec3 v2f_normal;\n"
    "varying vec3 v2f_view;\n"
    "uniform vec3 light1;\n"
    "uniform vec3 light2;\n"
    "uniform vec3 color;\n"
    "float p=100.0;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 L1 = normalize(light1);\n"
    "   vec3 L2 = normalize(light2);\n"
    "   vec3 N  = normalize(v2f_normal);\n"
    "   vec3 V  = normalize(v2f_view);\n"
    "   vec3 Rv = reflect(-V,N);\n"
    "   \n"
    "   float ambient  = 0.1;\n"
    "   float diffuse  = abs(dot(N,L1)) + abs(dot(N,L2));\n"
    "   float specular = max(0.0, pow(dot(Rv,L1),p)) + max(0.0, pow(dot(Rv,L2),p));\n"
    "   \n"
    "   vec3  rgb = color * (ambient + diffuse) + vec3(1,1,1) * specular;\n"
    "   \n"
    "   gl_FragColor = vec4(0,0,0, 1.0);\n"
    "}\n";

#endif

