
#include "helpers.h"

#include <windows.h>

#include <codecvt>

namespace ps {
namespace helpers {

using std::string;
using std::wstring;
using std::codecvt_utf8;
using std::wstring_convert;

string WideToUtf8(const wstring& wstr)
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

    return WideToUtf8(message);
}

string GetLastErrorMessage()
{
    return GetErrorMessage(static_cast<uint32_t>(::GetLastError()));
}


} // namespace helpers
} // namespace ps
