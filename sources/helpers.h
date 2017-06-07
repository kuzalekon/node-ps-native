
#pragma once

#ifndef PS_HELPERS_H
#define PS_HELPERS_H

#include <cstdint>
#include <string>

namespace ps {
namespace helpers {

#ifdef PS_WIN32
std::string WideToUtf8(const std::wstring& wstr);
#endif // PS_WIN32
std::string GetErrorMessage(uint32_t error);
std::string GetLastErrorMessage();

} // namespace helpers
} // namespace ps

#endif // PS_HELPERS_H
