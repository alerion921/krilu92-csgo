#include "aimbot.h"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../ConfigSystem.h"
#include "../Backtrack.h"
#include "../RuntimeSaver.hpp"
#include "../helpers\input.hpp"

void Aimbot::OnCreateMove(CUserCmd* cmd)
{
    C_BasePlayer* local = g_LocalPlayer;

    if (!local || !local->IsAlive())
    {
        return;
    }

    C_BaseCombatWeapon* weapon = local->m_hActiveWeapon().Get();
    if (!weapon || weapon->IsReloading())
    {
        return;
    }

    UpdateWeaponConfig(weapon);

    std::deque<int> hb_enabled;

    if (WeaponHitboxHead)
    {
        hb_enabled.push_back(HITBOX_HEAD);
    }
    if (WeaponHitboxNeck)
    {
        hb_enabled.push_back(HITBOX_NECK);
    }
    if (WeaponHitboxChest)
    {
        hb_enabled.push_back(HITBOX_CHEST);
    }
    if (WeaponHitboxPelvis)
    {
        hb_enabled.push_back(HITBOX_PELVIS);
    }
    if (WeaponHitboxStomach)
    {
        hb_enabled.push_back(HITBOX_STOMACH);
    }

    if (g_Config.GetBool("aim_backtrack"))
    {
        Backtrack::Get().LegitOnCreateMove(hb_enabled);
    }

    if (WeaponRcs)
    {
       ResetRecoil(cmd);
    }

    if (WeaponFov != 0.f)
    {
        DoAimbot(cmd, local, weapon);
    }

    if (WeaponRcs)
    {
       RemoveRecoil(local, cmd);
    }

    g_EngineClient->SetViewAngles(cmd->viewangles);
}


void Aimbot::ResetRecoil(CUserCmd* cmd)
{
	cmd->viewangles += LastAimpunchRemove;
}

void Aimbot::RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd)
{
        QAngle odata = local->m_aimPunchAngle();
        QAngle data = (odata + LastAimpunch);
        LastAimpunch = odata;
        data.pitch *= WeaponRecoilY;
        data.yaw *= WeaponRecoilX;
        cmd->viewangles -= data;
        LastAimpunchRemove = data;
}

void Aimbot::UpdateWeaponConfig(C_BaseCombatWeapon* weapon)
{
    if (weapon->IsPistol())
    {
        WeaponFov = g_Config.GetFloat("aim_pistol_fov");
        WeaponSmooth = g_Config.GetFloat("aim_pistol_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_pistol_randomize");
        WeaponDelay = g_Config.GetFloat("aim_pistol_delay");
        WeaponTriggerbot = g_Config.GetBool("aim_pistol_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_pistol_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_pistol_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_pistol_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_pistol_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_pistol_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_pistol_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_pistol_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_pistol_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_pistol_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_pistol_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_pistol_hitbox_foot");
    }
    else if (weapon->IsSniper())
    {
        WeaponFov = g_Config.GetFloat("aim_sniper_fov");
        WeaponSmooth = g_Config.GetFloat("aim_sniper_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_sniper_randomize");
        WeaponDelay = g_Config.GetFloat("aim_sniper_delay");
        WeaponTriggerbot = g_Config.GetBool("aim_sniper_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_sniper_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_sniper_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_sniper_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_sniper_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_sniper_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_sniper_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_sniper_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_sniper_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_sniper_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_sniper_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_sniper_hitbox_foot");
    }
    else if (weapon->IsSubmachinegun())
    {
        WeaponFov = g_Config.GetFloat("aim_smg_fov");
        WeaponSmooth = g_Config.GetFloat("aim_smg_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_smg_randomize");
        WeaponDelay = g_Config.GetFloat("aim_smg_delay");
        WeaponTriggerbot = g_Config.GetBool("aim_smg_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_smg_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_smg_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_smg_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_smg_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_smg_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_smg_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_smg_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_smg_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_smg_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_smg_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_smg_hitbox_foot");
    }
    else if (weapon->IsMachinegun())
    {
        WeaponFov = g_Config.GetFloat("aim_mg_fov");
        WeaponSmooth = g_Config.GetFloat("aim_mg_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_mg_randomize");
        WeaponDelay = g_Config.GetFloat("aim_mg_delay");
        WeaponTriggerbot = g_Config.GetBool("aim_mg_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_mg_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_mg_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_mg_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_mg_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_mg_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_mg_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_mg_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_mg_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_mg_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_mg_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_mg_hitbox_foot");
    }
    else if (weapon->IsShotgun())
    {
        WeaponFov = g_Config.GetFloat("aim_shotgun_fov");
        WeaponSmooth = g_Config.GetFloat("aim_shotgun_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_shotgun_randomize");
        WeaponDelay = g_Config.GetFloat("aim_shotgun_delay");

        WeaponTriggerbot = g_Config.GetBool("aim_shotgun_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_shotgun_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_shotgun_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_shotgun_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_shotgun_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_shotgun_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_shotgun_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_shotgun_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_shotgun_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_shotgun_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_shotgun_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_shotgun_hitbox_foot");
    }
    else
    {
        WeaponFov = g_Config.GetFloat("aim_rifle_fov");
        WeaponSmooth = g_Config.GetFloat("aim_rifle_smooth");
        WeaponRandomness = g_Config.GetFloat("aim_rifle_randomize");
        WeaponDelay = g_Config.GetFloat("aim_rifle_delay");
        WeaponTriggerbot = g_Config.GetBool("aim_rifle_triggerbot");

        WeaponRcs = g_Config.GetBool("aim_rifle_rcs");
        WeaponRecoilX = g_Config.GetFloat("aim_rifle_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("aim_rifle_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("aim_rifle_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("aim_rifle_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("aim_rifle_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("aim_rifle_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("aim_rifle_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("aim_rifle_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("aim_rifle_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("aim_rifle_hitbox_foot");
    }
}

int Aimbot::GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos)
{
    QAngle viewangles = cmd->viewangles;

    float BestFov = WeaponFov;
    Vector BestPos = Vector(0, 0, 0);
    int BestIndex = -1;
    //bool UsingBacktrack = false;
    //LegitTickRecord BestBacktrackTick;
    bool aim_backtrack = g_Config.GetBool("aim_backtrack");
    bool aim_backtrack_aim = g_Config.GetBool("aim_backtrack_aim");
    float aim_backtrack_ms = g_Config.GetFloat("aim_backtrack_ms");

    //float flRange = weapon->GetCSWeaponData()->flRange;

    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
        if (!entity || !entity->IsPlayer() || entity == local || entity->IsDormant()
            || !entity->IsAlive() || !entity->IsEnemy())
        {
            continue;
        }

        //Console.WriteLine("Found valid target")

        //if (local->m_vecOrigin().DistTo(entity->m_vecOrigin()) > flRange) continue;

        entity->PrecaceOptimizedHitboxes();

        for (int hitbox = 0; hitbox < HITBOX_MAX; hitbox++)
        {
            switch (hitbox)
            {
            case HITBOX_HEAD:
                if (!WeaponHitboxHead)
                {
                    continue;
                }
                break;
            case HITBOX_NECK:
                if (!WeaponHitboxNeck)
                {
                    continue;
                }
                break;
            case HITBOX_PELVIS:
                if (!WeaponHitboxPelvis)
                {
                    continue;
                }
                break;
            case HITBOX_STOMACH:
                if (!WeaponHitboxStomach)
                {
                    continue;
                }
                break;
            case HITBOX_LOWER_CHEST:
            case HITBOX_CHEST:
            case HITBOX_UPPER_CHEST:
                if (!WeaponHitboxChest)
                {
                    continue;
                }
                break;
            case HITBOX_RIGHT_THIGH:
            case HITBOX_LEFT_THIGH:
            case HITBOX_RIGHT_CALF:
            case HITBOX_LEFT_CALF:
                if (!WeaponHitboxLeg)
                {
                    continue;
                }
                break;
            case HITBOX_RIGHT_FOOT:
            case HITBOX_LEFT_FOOT:
                if (!WeaponHitboxFoot)
                {
                    continue;
                }
                break;
            case HITBOX_RIGHT_HAND:
            case HITBOX_LEFT_HAND:
            case HITBOX_RIGHT_UPPER_ARM:
            case HITBOX_RIGHT_FOREARM:
            case HITBOX_LEFT_UPPER_ARM:
            case HITBOX_LEFT_FOREARM:
                if (!WeaponHitboxArm)
                {
                    continue;
                }
                break;
            }

            Vector pos;
            if (!entity->GetOptimizedHitboxPos(hitbox, pos))
            {
                continue;
            }
            if (!local->CanSeePlayer(local, pos))
            {
                continue;
            }
            float fov = Math::GetFOV(viewangles, Math::CalcAngle(local->GetEyePos(), pos));

            if (fov < BestFov)
            {
                //UsingBacktrack = false;
                BestPos = pos;
                BestFov = fov;
                BestIndex = i;
            }
        }

        if (aim_backtrack && aim_backtrack_aim)
        {
            std::deque<LegitTickRecord> BacktrackRecords = Backtrack::Get().GetValidLegitRecords(i, aim_backtrack_ms);

            for (auto record = BacktrackRecords.begin(); record != BacktrackRecords.end(); record++)
            {
                for (int hitbox = 0; hitbox < HITBOX_MAX; hitbox++)
                {
                    switch (hitbox)
                    {
                    case HITBOX_HEAD:
                        if (!WeaponHitboxHead)
                        {
                            continue;
                        }
                        break;
                    case HITBOX_NECK:
                        if (!WeaponHitboxNeck)
                        {
                            continue;
                        }
                        break;
                    case HITBOX_PELVIS:
                        if (!WeaponHitboxPelvis)
                        {
                            continue;
                        }
                        break;
                    case HITBOX_STOMACH:
                        if (!WeaponHitboxStomach)
                        {
                            continue;
                        }
                        break;
                    case HITBOX_LOWER_CHEST:
                    case HITBOX_CHEST:
                    case HITBOX_UPPER_CHEST:
                        if (!WeaponHitboxChest)
                        {
                            continue;
                        }
                        break;
                    case HITBOX_RIGHT_THIGH:
                    case HITBOX_LEFT_THIGH:
                    case HITBOX_RIGHT_CALF:
                    case HITBOX_LEFT_CALF:
                    case HITBOX_RIGHT_FOOT:
                    case HITBOX_LEFT_FOOT:
                    case HITBOX_RIGHT_HAND:
                    case HITBOX_LEFT_HAND:
                    case HITBOX_RIGHT_UPPER_ARM:
                    case HITBOX_RIGHT_FOREARM:
                    case HITBOX_LEFT_UPPER_ARM:
                    case HITBOX_LEFT_FOREARM:
                        continue;
                        break;
                    }

                    if (!local->CanSeePlayer(local, record->hitboxes[hitbox]))
                    {
                        continue;
                    }
                    float fov = Math::GetFOV(viewangles, Math::CalcAngle(local->GetEyePos(), record->hitboxes[hitbox]));


                    if (fov < BestFov)
                    {
                        BestPos = record->hitboxes[hitbox];
                        BestFov = fov;
                        BestIndex = i;
                    }
                }
            }
        }
    }
    hitpos = BestPos;
    return BestIndex;
}

void Aimbot::DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
    static int LastEntity = -1;
    static float EntityFoundTime = 0.f;
    static bool DidLastShot = false;

    if (!weapon->HasBullets())
    {
        return;
    }

    if (!InputSys::Get().IsKeyDown(g_Config.GetInt("aim_key")))
    {
        return;
    }

    Vector Outpos = Vector(0, 0, 0);
    int Target = GetBestTarget(local, weapon, cmd, Outpos);
    if (Target == -1)
    {
        LastEntity = -1;
        EntityFoundTime = g_GlobalVars->curtime;
        return;
    }

    if (LastEntity != Target)
    {
        EntityFoundTime = g_GlobalVars->curtime;
        LastEntity = Target;
    }

    if (WeaponTriggerbot) {
        if (g_GlobalVars->curtime - EntityFoundTime > WeaponDelay)
        {
            cmd->buttons |= IN_ATTACK;
        }
        else if (weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER)
        {
            cmd->buttons &= ~IN_ATTACK;
        }
    }

    if (WeaponRandomness != 0.f)
    {
        Outpos += Vector(Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness));
    }

    QAngle CalcAng = Math::CalcAngle(local->GetEyePos(), Outpos);
    QAngle ViewAngle = cmd->viewangles;

    Math::NormalizeAngles(CalcAng);
    Math::NormalizeAngles(ViewAngle);

    QAngle Delta = ViewAngle - CalcAng;

    Math::NormalizeAngles(Delta);
    Math::ClampAngles(Delta);


    float RandomFactor = 1.f;

    QAngle FinalAngle = ViewAngle - (Delta / (WeaponSmooth * RandomFactor));

    Math::NormalizeAngles(FinalAngle);
    Math::ClampAngles(FinalAngle);

    if (DidLastShot && weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER && (weapon->IsPistol() || weapon->IsShotgun() || weapon->IsSniper()))
    {
        cmd->buttons &= ~IN_ATTACK;
    }
    DidLastShot = !DidLastShot;
    cmd->viewangles = FinalAngle;
    if (weapon->CanFire())
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(Target));
        if (!entity || !entity->IsPlayer() || entity == local || entity->IsDormant()
            || !entity->IsAlive() || !entity->IsEnemy())
        {
            return;
        }

        cmd->tick_count = TIME_TO_TICKS(entity->m_flSimulationTime() + Backtrack::Get().GetLerpTime());
    }
}