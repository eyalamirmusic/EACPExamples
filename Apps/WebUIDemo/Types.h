#pragma once

#include <Miro/Miro.h>
#include <string>

struct GreetRequest
{
    std::string name = {};

    MIRO_REFLECT(name)
};

struct GreetResponse
{
    std::string message = {};
    long long serverTimeMs = 0;

    MIRO_REFLECT(message, serverTimeMs)
};
