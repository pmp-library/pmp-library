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

#ifdef _WIN32
#  include <windows.h>
#else // Unix
#  include <sys/time.h>
#endif

#include <iostream>

//=============================================================================

namespace pmp {

//=============================================================================

//! A simple timer class.
class Timer
{
public:

    //! Constructor
    Timer()
        : m_elapsed(0.0), m_isRunning(false)
    {
#ifdef _WIN32 // Windows
        QueryPerformanceFrequency(&m_freq);
#endif
    }


    //! Start time measurement
    void start()
    {
        m_elapsed = 0.0;
        cont();
    }


    //! Continue measurement, accumulates elapased times
    void cont()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&m_startTime);
#else // Unix
        gettimeofday(&m_startTime, 0);
#endif
        m_isRunning = true;
    }


    //! Stop time measurement, return elapsed time in ms
    double stop()
    {
#ifdef _WIN32 // Windows
        QueryPerformanceCounter(&m_endTime);
        m_elapsed += ((double)(m_endTime.QuadPart - m_startTime.QuadPart)
                     / (double)m_freq.QuadPart * 1000.0f);
#else // Unix
        gettimeofday(&m_endTime, 0);
        m_elapsed += ((m_endTime.tv_sec  - m_startTime.tv_sec )*1000.0 +
                     (m_endTime.tv_usec - m_startTime.tv_usec)*0.001);
#endif
        m_isRunning = false;
        return elapsed();
    }


    //! Return elapsed time in ms (watch has to be stopped).
    double elapsed() const
    {
        if (m_isRunning)
        {
            std::cerr << "Timer: stop watch before calling elapsed()\n";
        }
        return m_elapsed;
    }


private:

    double  m_elapsed;
    bool    m_isRunning;

#ifdef _WIN32 // Windows
    LARGE_INTEGER m_startTime, m_endTime;
    LARGE_INTEGER m_freq;
#else // Unix
    timeval m_startTime, m_endTime;
#endif
};


//=============================================================================


//! output a timer to a stream
inline std::ostream&
operator<<(std::ostream& os, const Timer& timer)
{
    os << timer.elapsed() << " ms";
    return os;
}


//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
