//=============================================================================
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

#if defined _WIN32
#include <psapi.h>
#include <windows.h>
#elif defined __linux__
#include <sys/resource.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#elif defined __APPLE__
#include <mach/mach.h>
#endif

//=============================================================================

namespace surface_mesh {

//=============================================================================

//! A simple class to retrieve memory usage information.
class MemoryUsage
{
public:
    //! \brief Get the maximum memory size the application has used so far.
    //! \return the max. resident set size (RSS) in bytes
    static size_t maxSize();

    //! \brief Get the currently used memory.
    //! \return the current resident set size (RSS) in bytes
    static size_t currentSize();
};

//-----------------------------------------------------------------------------

size_t MemoryUsage::maxSize()
{
#if defined(_WIN32)

    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.PeakWorkingSetSize;

#elif defined(__linux__) || defined(__APPLE__)

    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);

#if defined(__APPLE__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024);
#endif

#endif
    return 0;
}

//-----------------------------------------------------------------------------

size_t MemoryUsage::currentSize()
{
#if defined(_WIN32)

    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;

#elif defined(__linux__)

    long  rss = 0;
    FILE* fp  = nullptr;

    if ((fp = fopen("/proc/self/statm", "r")) == nullptr)
    {
        std::cerr << "Failed to read process information file" << std::endl;
        return 0;
    }

    if (fscanf(fp, "%*s%ld", &rss) != 1)
    {
        std::cerr << "Failed to retrieve RSS information" << std::endl;
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);

#elif defined(__APPLE__)

    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    auto ret = task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount);
    if (ret != KERN_SUCCESS)
    {
        std::cerr << "Failed to retrieve task information" << std::endl;
        return 0;
    }
    return (size_t)info.resident_size;

#else
    return 0;
#endif
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
