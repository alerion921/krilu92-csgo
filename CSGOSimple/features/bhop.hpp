#pragma once

#include "../helpers/math.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../MovementFix.h"

class Movement : public Singleton<Movement>
{
public:
    void Bunnyhop(CUserCmd* cmd);
    void Autostrafe(CUserCmd* cmd, QAngle va);
};