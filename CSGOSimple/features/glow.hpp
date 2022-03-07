#pragma once

#include "../Singleton.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"

class Glow
    : public Singleton<Glow>
{
    friend class Singleton<Glow>;

    Glow();
    ~Glow();

public:
    void Run();
    void Shutdown();
};