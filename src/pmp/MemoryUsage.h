// Copyright 2017-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

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

namespace pmp {

//! A simple class to retrieve memory usage information.
//! \ingroup core
class MemoryUsage
{
public:
    //! \brief Get the maximum memory size the application has used so far.
    //! \return the max. resident set size (RSS) in bytes
    static size_t max_size();

    //! \brief Get the currently used memory.
    //! \return the current resident set size (RSS) in bytes
    static size_t current_size();
};

size_t MemoryUsage::max_size()
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

size_t MemoryUsage::current_size()
{
#if defined(_WIN32)

    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;

#elif defined(__linux__)

    long rss = 0;
    FILE* fp = nullptr;

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
    auto ret = task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                         (task_info_t)&info, &infoCount);
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

} // namespace pmp
