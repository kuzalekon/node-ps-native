
#include "process.h"

namespace ps {
namespace native{

using std::runtime_error;
using std::string;
using std::list;

namespace { // anonymous

string GetErrorMessage(::DWORD error)
{
    ::LPSTR buffer = nullptr;
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                     FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, 
                     MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buffer, 0, nullptr);

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

    ::HANDLE snapshot32 = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot32) {
        throw runtime_error(GetLastErrorMessage());
    }

    ::PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);
    if (FALSE == ::Process32First(snapshot32, &pe32)) {
        ::CloseHandle(snapshot32);
        throw runtime_error(GetLastErrorMessage());
    }

    do {
        Process process;
        process.pid = pe32.th32ProcessID;
        process.ppid = pe32.th32ParentProcessID;
        process.exe = pe32.szExeFile;
        process.priority = pe32.pcPriClassBase;
        process.threads = pe32.cntThreads;

        processes.push_back(process);
    }
    while (FALSE != ::Process32Next(snapshot32, &pe32));

    ::CloseHandle(snapshot32);

    return processes;
}

} // native namespace
} // ps namespace
