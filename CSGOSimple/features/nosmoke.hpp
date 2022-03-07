#pragma once

#include "../Singleton.hpp"
#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"

class NoSmoke : public Singleton<NoSmoke>
{
public:
    void RemoveSmoke();
};

