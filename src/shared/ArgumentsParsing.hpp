#pragma once

#include <optional>
#include <stdexcept>
#include <cstring>
#include <array>
#include <cstdint>

namespace PPNet
{

static int StoiWithErrorPrefix(const char *str, const char *errorMessagePrefix)
{
    try
    {
        int res = std::stoi(str);
        return res;
    }
    catch (const std::invalid_argument &e)
    {
        std::string parseErrorMsg(e.what());
        std::string errorMsg = errorMessagePrefix + parseErrorMsg;
        throw std::invalid_argument{ errorMsg };
    }
}
}
