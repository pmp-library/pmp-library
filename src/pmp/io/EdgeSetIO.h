//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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

#include <pmp/EdgeSet.h>
#include <pmp/io/IOOptions.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup io io
//! @{

//=============================================================================

//! Class for reading and writing edge sets from and to files.
//!
//! This class currently supports only the custom KNT format. This is an ASCII
//! format basically storing vertex positions and edge indices.
class EdgeSetIO
{
public:
    //! Construct with default IO opttions
    EdgeSetIO(const IOOptions& options = IOOptions()) : m_options(options){};

    //! \brief read edge set from file \c filename into \c es
    //! \note any previous data in \c es will be removed
    bool read(EdgeSet& es, const std::string& filename);

    //! \brief write edge set \c es to file \c filename
    bool write(const EdgeSet& es, const std::string& filename);

private:
    static bool readKNT(EdgeSet& es, const std::string& filename);
    static bool writeKNT(const EdgeSet& es, const std::string& filename);

private:
    IOOptions m_options;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
