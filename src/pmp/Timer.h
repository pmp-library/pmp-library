//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <chrono>
#include <iostream>

//=============================================================================

namespace pmp {

//=============================================================================

//! A simple timer class.
class Timer
{
public:
    //! Constructor
    Timer() : m_elapsed(0.0), m_isRunning(false) {}

    //! Start time measurement
    void start()
    {
        m_elapsed = 0.0;
        cont();
    }

    //! Continue measurement, accumulates elapased times
    void cont()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
        m_isRunning = true;
    }

    //! Stop time measurement, return elapsed time in ms
    Timer& stop()
    {
        m_endTime = std::chrono::high_resolution_clock::now();
        duration time_span =
            std::chrono::duration_cast<duration>(m_endTime - m_startTime);
        m_elapsed += time_span.count();
        m_isRunning = false;
        return *this;
    }

    //! Return elapsed time in ms (watch has to be stopped).
    double elapsed() const
    {
        if (m_isRunning)
        {
            std::cerr << "Timer: stop watch before calling elapsed()\n";
        }
        return 1000.0 * m_elapsed;
    }

private:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock>
        time_point;
    typedef std::chrono::duration<double> duration;

    time_point m_startTime, m_endTime;
    double m_elapsed;
    bool m_isRunning;
};

//=============================================================================

//! output a timer to a stream
inline std::ostream& operator<<(std::ostream& os, const Timer& timer)
{
    os << timer.elapsed() << " ms";
    return os;
}

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
