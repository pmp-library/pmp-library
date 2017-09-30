//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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
#pragma once
//=============================================================================

// Mac OS X
#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

// Windows
#elif _WIN32
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#include <GL/glew.h>
#include <GL/glu.h>
#include <stdlib.h>

// Unix
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#include <stdlib.h>
#include <iostream>


//=============================================================================


//! Check for OpenGL errors.
inline void glCheckError(bool _force=false)
{
  GLenum error = glGetError();

  if (error == GL_NO_ERROR)
    {
      if (_force)
        {
          std::cout << " No GL error!" << std::endl;
        }
    }

  else
    {
      do
        {
          switch (error)
            {
            case GL_INVALID_ENUM:
              std::cerr << " GL error: invalid enum\n";
              break;

            case GL_INVALID_VALUE:
              std::cerr << " GL error: invalid value (out of range)\n";
              break;

            case GL_INVALID_OPERATION:
              std::cerr << " GL error: invalid operation (not allowed in current state)\n";
              break;

            case GL_INVALID_FRAMEBUFFER_OPERATION:
              std::cerr << " GL error: invalid framebuffer operation (framebuffer not complete)\n";
              break;

            case GL_OUT_OF_MEMORY:
              std::cerr << " GL error: out of memory\n";
              break;

            case GL_STACK_UNDERFLOW:
              std::cerr << " GL error: stack underflow\n";
              break;

            case GL_STACK_OVERFLOW:
              std::cerr << " GL error: stack overflow\n";
              break;
            }

          error = glGetError();
        }
      while (error != GL_NO_ERROR);
    }
}


//=============================================================================
