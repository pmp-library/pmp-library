// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <chrono>
#include <iostream>

namespace pmp {

//! A simple timer class.
//! \ingroup core
class Timer
{
public:
    //! Constructor
    Timer() = default;

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
    using hclock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<hclock>;
    using duration = std::chrono::duration<double>;

    time_point start_time_, end_time_;
    double elapsed_{0.0};
    bool is_running_{false};
};

//! output a timer to a stream
inline std::ostream& operator<<(std::ostream& os, const Timer& timer)
{
    os << timer.elapsed() << " ms";
    return os;
}

} // namespace pmp
