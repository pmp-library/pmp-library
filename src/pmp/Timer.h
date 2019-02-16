//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <chrono>
#include <iostream>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup core core
//!@{

//! A simple timer class.
class Timer
{
public:
    //! Constructor
    Timer() : elapsed_(0.0), is_running_(false) {}

    //! Start time measurement
    void start()
    {
        elapsed_ = 0.0;
        cont();
    }

    //! Continue measurement, accumulates elapased times
    void cont()
    {
        start_time_ = hclock::now();
        is_running_ = true;
    }

    //! Stop time measurement, return elapsed time in ms
    Timer& stop()
    {
        using std::chrono::duration_cast;
        end_time_ = hclock::now();
        duration time_span = duration_cast<duration>(end_time_ - start_time_);
        elapsed_ += time_span.count();
        is_running_ = false;
        return *this;
    }

    //! Return elapsed time in ms (watch has to be stopped).
    double elapsed() const
    {
        if (is_running_)
        {
            std::cerr << "Timer: stop timer before calling elapsed()\n";
        }
        return 1000.0 * elapsed_;
    }

private:
    typedef std::chrono::high_resolution_clock hclock;
    typedef std::chrono::time_point<hclock> time_point;
    typedef std::chrono::duration<double> duration;

    time_point start_time_, end_time_;
    double elapsed_;
    bool is_running_;
};

//=============================================================================

//! output a timer to a stream
inline std::ostream& operator<<(std::ostream& os, const Timer& timer)
{
    os << timer.elapsed() << " ms";
    return os;
}

//=============================================================================
//!@}
//=============================================================================
} // namespace pmp
//=============================================================================
