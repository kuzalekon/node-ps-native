
#include "process.h"
#include "helpers.h"

#include <stdexcept>
#include <string>

namespace ps {
namespace native{

using std::runtime_error;
using std::string;
using std::wstring;
using std::list;

ProcessList List()
{
    ProcessList processes;

    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        throw runtime_error(helpers::GetLastErrorMessage());
    }

    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(pe);
    if (FALSE == ::Process32FirstW(snapshot, &pe)) {
        ::CloseHandle(snapshot);
        throw runtime_error(helpers::GetLastErrorMessage());
    }

    do {
        Process process;
        process.pid = static_cast<uint32_t>(pe.th32ProcessID);
        process.parent = static_cast<uint32_t>(pe.th32ParentProcessID);
        process.priority = static_cast<uint32_t>(pe.pcPriClassBase);
        process.threads = static_cast<uint32_t>(pe.cntThreads);

        const wstring name(pe.szExeFile);
        ::StringCchCopyA(&process.name[0], MAX_PATH, &helpers::WideToUtf8(name)[0]);

        processes.push_back(process);
    }
    while (FALSE != ::Process32NextW(snapshot, &pe));

    ::CloseHandle(snapshot);

    return processes;
}

void Kill(uint32_t pid, int32_t code)
{
    HANDLE process = ::OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<::DWORD>(pid));
    if (NULL == process) {
        throw runtime_error(helpers::GetLastErrorMessage());
    }

    static const DWORD timeout = 1000;
    DWORD result = WAIT_OBJECT_0;
    while (WAIT_OBJECT_0 == result) {
        result = ::WaitForSingleObject(process,timeout);
        if (FALSE == ::TerminateProcess(process, static_cast<UINT>(code))) {
            throw runtime_error(helpers::GetLastErrorMessage());
        }
    }
    
    ::CloseHandle(process);
}

} // native namespace
} // ps namespace
