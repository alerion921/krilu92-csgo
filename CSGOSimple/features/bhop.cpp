#include "bhop.hpp"

void Movement::Bunnyhop(CUserCmd* cmd)
{
  static bool jumped_last_tick = false;
  static bool should_fake_jump = false;

  if (!g_LocalPlayer)
	  return;

  if (!g_LocalPlayer->IsAlive())
	  return;

  if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
	  return;

  if (g_LocalPlayer->m_fFlags() & FL_INWATER)
	  return;

  if(!jumped_last_tick && should_fake_jump) {
    should_fake_jump = false;
    cmd->buttons |= IN_JUMP;
  } else if(cmd->buttons & IN_JUMP) {
    if(g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
      jumped_last_tick = true;
      should_fake_jump = true;
    } else {
      cmd->buttons &= ~IN_JUMP;
      jumped_last_tick = false;
    }
  } else {
    jumped_last_tick = false;
    should_fake_jump = false;
  }
}

void Movement::Autostrafe(CUserCmd* cmd, QAngle va)
{
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
    {
        return;
    }

    static bool leftRight;
    bool inMove = cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT;

    if (cmd->buttons & IN_FORWARD && g_LocalPlayer->m_vecVelocity().Length() <= 50.0f)
    {
        cmd->forwardmove = 250.0f;
    }

    float yaw_change = 0.0f;
    if (g_LocalPlayer->m_vecVelocity().Length() > 50.f)
    {
        yaw_change = 30.0f * fabsf(30.0f / g_LocalPlayer->m_vecVelocity().Length());
    }

    C_BaseCombatWeapon* ActiveWeapon = g_LocalPlayer->m_hActiveWeapon();
    if (ActiveWeapon && ActiveWeapon->CanFire() && cmd->buttons & IN_ATTACK)
    {
        yaw_change = 0.0f;
    }

    QAngle viewAngles = va;

    bool OnGround = (g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    if (!OnGround && !inMove)
    {
        if (leftRight || cmd->mousedx > 1)
        {
            viewAngles.yaw += yaw_change;
            cmd->sidemove = 350.0f;
        }
        else if (!leftRight || cmd->mousedx < 1)
        {
            viewAngles.yaw -= yaw_change;
            cmd->sidemove = -350.0f;
        }

        leftRight = !leftRight;
    }
    viewAngles.Normalize();
    Math::ClampAngles(viewAngles);
    MovementFix::Get().Correct(viewAngles, cmd, cmd->forwardmove, cmd->sidemove);
}