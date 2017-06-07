
#pragma once

#ifndef PS_PROCESS_H
#define PS_PROCESS_H

#if defined(PS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#elif defined(PS_LINUX)
#endif

#include <cstdint>
#include <string>
#include <list>

namespace ps 
{

class Process
{
public:
    enum InfoSet
    {
        Pid = 1,
        Parent = 2,
        Name = 4,
        Priority = 8,
        Threads = 16,
        EmptyInfo = 0,
        FullInfo = 31
    };

    typedef struct
    {
        uint32_t pid;
        uint32_t parent;
        std::string name;
        uint32_t priority;
        uint32_t threads;
    } Info;

    typedef std::list<Info> List;

public:
    Process() = default;
    Process(const Process& other) = delete;

public:
    Process& operator=(const Process& rhs) = delete;

public:
    static List Enum(uint8_t infoset);

    static bool Find(uint32_t pid);
    static bool Find(const std::string& mask);

    static void Kill(uint32_t pid, uint32_t code = 0);
}; // class Process

} // ps namespace

#endif // PS_PROCESS_H