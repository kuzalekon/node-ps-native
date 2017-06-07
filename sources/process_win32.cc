
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

string GetErrorMessage(uint32_t error)
{
    LPWSTR buffer = nullptr;
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER |
                     FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, static_cast<DWORD>(error),
                     MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                     reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

    wstring message = std::to_wstring(error) + L' ';
    if (nullptr != buffer) {
        message += buffer;
        ::HeapFree(::GetProcessHeap(), HEAP_ZERO_MEMORY, buffer);
    }

    return ws2utf8(message);
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
        Info info = { 0 };
        if (0 != (infoset & Pid))
            info.pid = static_cast<uint32_t>(entry.th32ProcessID);
        if (0 != (infoset & Parent))
            info.parent = static_cast<uint32_t>(entry.th32ParentProcessID);
        if (0 != (infoset & Name))
            info.name = ws2utf8(entry.szExeFile);
        if (0 != (infoset & Priority))
            info.priority = static_cast<uint32_t>(entry.pcPriClassBase);
        if (0 != (infoset & Threads))
            info.threads = static_cast<uint32_t>(entry.cntThreads);

        processes.push_back(info);
    }
    while (FALSE != ::Process32NextW(snapshot, &entry));

    ::CloseHandle(snapshot);

    return processes;
}

} // ps namespace
