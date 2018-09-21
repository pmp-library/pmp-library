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

#include <pmp/PointSet.h>
#include <pmp/io/IOOptions.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup io io
//! @{

//=============================================================================

//! Class for reading and writing point sets from and to files.
//!
//! This class supports XYZ file format for reading and writing. Only ASCII mode
//! is supported. Only supports point coordinates and normals.
//!
//! The class also supports reading colored point clouds from Agisoft.
//! Format: point coordinates, RGB colors, and normals per line. ASCII only,
//! file extension ".agi"
class PointSetIO
{
public:
    //! Construct with default options
    PointSetIO(const IOOptions& options = IOOptions()) : m_options(options){};

    //! Read point set from file \c filename
    //! \note any previous data in \c ps will be removed
    bool read(PointSet& ps, const std::string& filename);

    //! Write point set \c ps to file \c filename
    bool write(const PointSet& ps, const std::string& filename);

protected:
    //! \brief read point set from \c filename
    bool readXYZ(PointSet& ps, const std::string& filename);

    //! \brief write point set from \c filename
    bool writeXYZ(const PointSet& ps, const std::string& filename);

    //! \brief read Agisoft point set from \c filename
    bool readAGI(PointSet& ps, const std::string& filename);

private:
    IOOptions m_options;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
