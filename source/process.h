
#pragma once

#ifndef PS_PROCESS_H
#define PS_PROCESS_H

#if defined(PS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#elif defined(PS_LINUX)
#endif

#include <stdexcept>
#include <cstdint>
#include <string>
#include <list>

namespace ps {
namespace native {

#if defined(PS_WIN32)
typedef DWORD pid_t;
#elif defined(PS_LINUX)
#endif

typedef struct
{
    pid_t pid = 0;
    pid_t ppid = 0;
    std::string exe = "";
    uint32_t priority = 0;
    uint32_t threads = 0;
} Process;

typedef std::list<Process> ProcessList;

ProcessList List();
//ProcessList Find(const std::string& name);
//ProcessList Find(pid_t pid);
//void Kill(pid_t pid);

} // native namespace
} // ps namespace

#endif // PS_PROCESS_H