//=============================================================================
// Copyright (C) 2011 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
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

#include <cassert>
#include <cstdlib>
#include <iostream>

#include <surface_mesh/Timer.h>
#include <surface_mesh/types.h>

//=============================================================================

class PerformanceTest
{
public:
    PerformanceTest() {}

    void run(const char* input, const char* output)
    {
        surface_mesh::Timer timer;

        timer.start();
        if (!read_mesh(input))
        {
            std::cerr << "read error\n";
            exit(1);
        }
        timer.stop();
        std::cout << "Read mesh   : " << timer << std::endl;

        timer.start();
        int c;
        for (int i = 0; i < 100; ++i)
            c = circulator_test();
        timer.stop();
        SM_ASSERT(c == 0);
        std::cout << "Circulator  : " << timer << std::endl;

        timer.start();
        for (int i = 0; i < 1000; ++i)
            barycenter_test();
        timer.stop();
        std::cout << "Barycenter  : " << timer << std::endl;

        timer.start();
        for (int i = 0; i < 100; ++i)
            normal_test();
        timer.stop();
        std::cout << "Normals     : " << timer << std::endl;

        timer.start();
        for (int i = 0; i < 100; ++i)
            smoothing_test();
        timer.stop();
        std::cout << "Smoothing   : " << timer << std::endl;

        timer.start();
        subdivision_test();
        timer.stop();
        std::cout << "Subdivision : " << timer << std::endl;

        timer.start();
        collapse_test();
        timer.stop();
        std::cout << "Collapse    : " << timer << std::endl;

        timer.start();
        if (!write_mesh(output))
        {
            std::cerr << "write error\n";
            exit(1);
        }
        timer.stop();
        std::cout << "Write mesh  : " << timer << std::endl;

        std::cout << std::endl;
    }

protected:
    virtual bool read_mesh(const char* filename)  = 0;
    virtual bool write_mesh(const char* filename) = 0;
    virtual int  circulator_test() { return 0; }
    virtual void barycenter_test() {}
    virtual void normal_test() {}
    virtual void smoothing_test() {}
    virtual void subdivision_test() {}
    virtual void collapse_test() {}
};

//=============================================================================
