
#include "process.h"

#include <stdexcept>
#include <regex>
#include <algorithm>

namespace ps {

using std::runtime_error;
using std::string;
using std::list;
using std::regex;

namespace { // anonymous

string GetErrorMessage(int32_t error)
{
    return string(::strerror(error));
}

string GetLastErrorMessage()
{
    return GetErrorMessage(static_cast<int32_t>(errno));
}

bool IsPidDir(const string& name)
{
    return (string::npos == name.find_first_not_of(u8"0123456789"));
}

} // anonymous namespace

Process::List Process::Enum(uint8_t infoset)
{
    static const uint8_t kPathSize = 32;
    static const uint16_t kFullPathSize = 1024;

    static const char* const kProcPath = u8"/proc";
    static const char* const kStatPath = u8"/proc/%s/stat";
    static const char* const kExePath = u8"/proc/%s/exe";

    struct dirent* de = nullptr;
    DIR* dir = nullptr;
    FILE* file = nullptr;
    string path(kPathSize, '\0');
    string fullPath(kFullPathSize, '\0');

    dir = ::opendir(kProcPath);
    if (nullptr == dir) {
        throw runtime_error(GetLastErrorMessage());
    }

    List processes;
    while (nullptr != (de = ::readdir(dir))) {
        if (DT_DIR != de->d_type || !IsPidDir(de->d_name))
            continue;

        std::fill(path.begin(), path.end(), '\0');
        ::snprintf(&path[0], kPathSize, kStatPath, de->d_name);

        file = ::fopen(path.c_str(), "r");
        if (nullptr == file) {
            throw runtime_error(GetLastErrorMessage());
        }

        Info process = { 0 };
        if (0 != (infoset & Pid)) ::fscanf(file, "%d", &process.pid);               // 1
        else ::fscanf(file, "%*d");
        ::fscanf(file, " %*s");
        ::fscanf(file, " %*c");
        if (0 != (infoset & Parent)) ::fscanf(file, " %d", &process.parent);        // 4
        else ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*u");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        if (0 != (infoset & Priority)) ::fscanf(file, " %d", &process.priority);    // 18
        else ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        if (0 != (infoset & Threads)) ::fscanf(file, " %d", &process.threads);      // 20
        else ::fscanf(file, " %*d");
        ::fscanf(file, " %*d");
        ::fscanf(file, " %*u");
        ::fclose(file);

        if (0 != (infoset & Name)) {
            std::fill(path.begin(), path.end(), '\0');
            std::fill(fullPath.begin(), fullPath.end(), '\0');
            ::snprintf(&path[0], kPathSize, kExePath, de->d_name);
            const ssize_t size = ::readlink(path.c_str(), &fullPath[0], fullPath.size());
            if (-1 != size) {
                fullPath[size] = '\0';
                process.name = ::basename(&fullPath[0]);
            }
        }

        processes.push_back(process);
    }

    ::closedir(dir);

    return processes;
}

bool Process::Find(uint32_t pid, Info& process, uint8_t infoset)
{
    List snapshot = Enum(Pid | infoset);
    for (auto entry : snapshot) {
        if (pid == entry.pid) {
            if (0 != (infoset & Pid))
                process.pid = entry.pid;
            if (0 != (infoset & Parent))
                process.parent = entry.parent;
            if (0 != (infoset & Name))
                process.name = entry.name;
            if (0 != (infoset & Priority))
                process.priority = entry.priority;
            if (0 != (infoset & Threads))
                process.threads = entry.threads;

            return true;
        }
    }

    return false;
}

bool Process::Find(const std::string& mask, List& processes, uint8_t infoset)
{
    bool found = false;
    List snapshot = Enum(Name | infoset);
    for (auto entry : snapshot) {
        if (true == std::regex_match(entry.name, std::regex(mask))) {
            Info process = { 0 };
            if (0 != (infoset & Pid))
                process.pid = entry.pid;
            if (0 != (infoset & Parent))
                process.parent = entry.parent;
            if (0 != (infoset & Name))
                process.name = entry.name;
            if (0 != (infoset & Priority))
                process.priority = entry.priority;
            if (0 != (infoset & Threads))
                process.threads = entry.threads;

            processes.push_back(process);
            found = true;
        }
    }

    return found;
}

void Process::Kill(uint32_t pid, int32_t code)
{
    if (-1 == ::kill(pid, code)) {
        throw runtime_error(GetLastErrorMessage());
    }
}

} // namespace ps
