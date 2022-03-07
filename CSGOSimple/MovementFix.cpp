
#include "MovementFix.h"
#include "helpers/math.hpp"

void MovementFix::Correct(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
    float deltaView = pCmd->viewangles.yaw - vOldAngles.yaw;
    float f1;
    float f2;

    if (vOldAngles.yaw < 0.f)
    {
        f1 = 360.0f + vOldAngles.yaw;
    }
    else
    {
        f1 = vOldAngles.yaw;
    }

    if (pCmd->viewangles.yaw < 0.0f)
    {
        f2 = 360.0f + pCmd->viewangles.yaw;
    }
    else
    {
        f2 = pCmd->viewangles.yaw;
    }

    if (f2 < f1)
    {
        deltaView = abs(f2 - f1);
    }
    else
    {
        deltaView = 360.0f - abs(f1 - f2);
    }
    deltaView = 360.0f - deltaView;

    pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
    pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;

    pCmd->buttons &= ~IN_MOVERIGHT;
    pCmd->buttons &= ~IN_MOVELEFT;
    pCmd->buttons &= ~IN_FORWARD;
    pCmd->buttons &= ~IN_BACK;
}
