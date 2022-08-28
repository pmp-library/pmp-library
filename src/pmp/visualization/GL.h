// Copyright 2011-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

// Windows
#if _WIN32
#include <Windows.h>
#endif

#include <GL/glew.h>

#include <cstdlib>
#include <iostream>

//! Check for OpenGL errors.
inline void glCheckError(bool force = false)
{
    GLenum error = glGetError();

    if (error == GL_NO_ERROR)
    {
        if (force)
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
                    std::cerr << " GL error: invalid operation (not allowed in "
                                 "current state)\n";
                    break;

                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    std::cerr << " GL error: invalid framebuffer operation "
                                 "(framebuffer not complete)\n";
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
        } while (error != GL_NO_ERROR);
    }
}
