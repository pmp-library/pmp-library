// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <chrono>
#include <iostream>

namespace pmp {

//! A simple stop watch class.
//! \ingroup core
class StopWatch
{
public:
    //! Start time measurement.
    void start()
    {
        elapsed_ = 0.0;
        resume();
    }

    //! Continue measurement, accumulates elapsed times.
    void resume()
    {
        start_time_ = hclock::now();
        is_running_ = true;
    }

    //! Stop time measurement.
    StopWatch& stop()
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
            std::cerr << "StopWatch: stop timer before calling elapsed()\n";
        }
        return 1000.0 * elapsed_;
    }

private:
    using hclock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<hclock>;
    using duration = std::chrono::duration<double>;

    time_point start_time_;
    time_point end_time_;
    double elapsed_{0.0};
    bool is_running_{false};
};

//! output a elapsed time to a stream
inline std::ostream& operator<<(std::ostream& os, const StopWatch& watch)
{
    os << watch.elapsed() << " ms";
    return os;
}

} // namespace pmp
