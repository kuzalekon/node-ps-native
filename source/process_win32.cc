
#include "process.h"

#include <stdexcept>
#include <string>

namespace ps {
namespace native{

using std::runtime_error;
using std::string;
using std::list;

namespace { // anonymous

string GetErrorMessage(::DWORD error)
{
    LPSTR buffer = nullptr;
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                     FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, 
                     MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), 
                     reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

    string message = "Unknown error";
    if (nullptr != buffer) {
        message = buffer;
        ::HeapFree(::GetProcessHeap(), HEAP_ZERO_MEMORY, buffer);
    }

    return message;
}

string GetLastErrorMessage()
{
    return GetErrorMessage(::GetLastError());
}

} // anonymous namespace

ProcessList List()
{
    ProcessList processes;

    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        throw runtime_error(GetLastErrorMessage());
    }

    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(pe);
    if (FALSE == ::Process32First(snapshot, &pe)) {
        ::CloseHandle(snapshot);
        throw runtime_error(GetLastErrorMessage());
    }

    do {
        Process process;
        process.pid = static_cast<uint32_t>(pe.th32ProcessID);
        process.parent = static_cast<uint32_t>(pe.th32ParentProcessID);
        process.priority = static_cast<uint32_t>(pe.pcPriClassBase);
        process.threads = static_cast<uint32_t>(pe.cntThreads);

        ::StringCchCopyA(&process.name[0], MAX_PATH, &pe.szExeFile[0]);

        processes.push_back(process);
    }
    while (FALSE != ::Process32Next(snapshot, &pe));

    ::CloseHandle(snapshot);

    return processes;
}

void Kill(uint32_t pid, int32_t signal)
{
    HANDLE process = ::OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<::DWORD>(pid));
    if (NULL == process) {
        throw runtime_error(GetLastErrorMessage());
    }

    static const DWORD timeout = 1000;
    DWORD result = WAIT_OBJECT_0;
    while (WAIT_OBJECT_0 == result) {
        result = ::WaitForSingleObject(process,timeout);
        if (FALSE == ::TerminateProcess(process, static_cast<::UINT>(signal))) {
            throw runtime_error(GetLastErrorMessage());
        }
    }
    
    ::CloseHandle(process);
}

} // native namespace
} // ps namespace
