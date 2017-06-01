
#pragma once

#ifndef PS_PROCESS_H
#define PS_PROCESS_H

#if defined(PS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#include <tlhelp32.h>
#elif defined(PS_LINUX)
#endif

#include <cstdint>
#include <list>

#ifndef MAX_PATH
#define MAX_PATH    256
#endif // MAX_PATH

namespace ps {
namespace native {

enum ProcessProperties
{
    None = 0,
    Pid = 1,
    Name = 2,
    Parent = 4,
    Priority = 8,
    Threads = 16,
    All = 31
};

typedef struct
{
    uint32_t pid = 0;
    char name[MAX_PATH] = { '\0' };
    uint32_t parent = 0;
    uint32_t priority = 0;
    uint32_t threads = 0;
} Process;

typedef std::list<Process> ProcessList;

ProcessList List();
//ProcessList Find(const std::string& name);
//ProcessList Find(pid_t pid);
void Kill(uint32_t pid, int32_t signal);

} // native namespace
} // ps namespace

#endif // PS_PROCESS_H