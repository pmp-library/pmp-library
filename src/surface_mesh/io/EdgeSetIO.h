//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
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

#include <surface_mesh/EdgeSet.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

class EdgeSetIO
{
public:
    //! \brief read edge set from file \c filename into \c es
    //! \note any previous data in \c es will be removed
    static bool read(EdgeSet& es, const std::string& filename);

    //! \brief write edge set \c es to file \c filename
    static bool write(const EdgeSet& es, const std::string& filename)
    {
        SM_ASSERT(es.nVertices() > 0);
        SM_ASSERT(filename.length() > 0);
        return false;
    }

private:
    static bool readKNT(EdgeSet& ps, const std::string& filename);
};

//=============================================================================
} // namespace surface_mesh
//=============================================================================
