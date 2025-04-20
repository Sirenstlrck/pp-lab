#pragma once

#include <string>

namespace PPNet
{
template<typename Exception>
void ThrowWithErrnoStr(std::string prefix)
{
    std::string errnoStr(std::strerror(errno));
    throw Exception(prefix + "; Errno: '" + std::to_string(errno) + "' " + errnoStr);
}
}