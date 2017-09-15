
#include "process.h"

#include <stdexcept>
#include <string>
#include <codecvt>
#include <regex>

namespace ps {

using std::runtime_error;
using std::string;
using std::wstring;
using std::wstring_convert;
using std::codecvt_utf8;
using std::list;
using std::regex;

namespace { // anonymous

string ws2utf8(const wstring& wstr)
{
    return wstring_convert<codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wstr);
}

const char* GetErrorMessage(uint32_t error)
{
    static const size_t length = 150;
    static char message[length];
    
    ::ZeroMemory(message, '\0', length);
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, static_cast<DWORD>(error),
                     MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                     reinterpret_cast<LPSTR>(&buffer),
                     static_cast<DWORD>(length), nullptr);

    return message;
}

string GetLastErrorMessage()
{
    return GetErrorMessage(static_cast<uint32_t>(::GetLastError()));
}

} // anonymous namespace

Process::List Process::Enum(uint8_t infoset)
{
    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        throw runtime_error(GetLastErrorMessage());
    }

    PROCESSENTRY32W entry = { 0 };
    entry.dwSize = sizeof(entry);
    if (FALSE == ::Process32FirstW(snapshot, &entry)) {
        ::CloseHandle(snapshot);
        throw runtime_error(GetLastErrorMessage());
    }

    List processes;
    do {
        Info process = { 0 };
        if (0 != (infoset & Pid))
            process.pid = static_cast<uint32_t>(entry.th32ProcessID);
        if (0 != (infoset & Parent))
            process.parent = static_cast<uint32_t>(entry.th32ParentProcessID);
        if (0 != (infoset & Name))
            process.name = ws2utf8(entry.szExeFile);
        if (0 != (infoset & Priority))
            process.priority = static_cast<uint32_t>(entry.pcPriClassBase);
        if (0 != (infoset & Threads))
            process.threads = static_cast<uint32_t>(entry.cntThreads);

        processes.push_back(process);
    }
    while (FALSE != ::Process32NextW(snapshot, &entry));

    ::CloseHandle(snapshot);

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
    HANDLE process = ::OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<::DWORD>(pid));
    if (NULL == process) {
        throw runtime_error(GetLastErrorMessage());
    }

    static const DWORD timeout = 3000; // 3 sec
    DWORD result = WAIT_OBJECT_0;
    while (WAIT_OBJECT_0 == result) {
        result = ::WaitForSingleObject(process, timeout);
        if (FALSE == ::TerminateProcess(process, static_cast<UINT>(code))) {
            throw runtime_error(GetLastErrorMessage());
        }
    }

    ::CloseHandle(process);
}

} // ps namespace
