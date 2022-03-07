
#include "../singleton.hpp"
#include "../valve_sdk/csgostructs.hpp"

#pragma once

class Aimbot : public Singleton<Aimbot>
{
public:
	void OnCreateMove(CUserCmd* cmd);
private:
	void UpdateWeaponConfig(C_BaseCombatWeapon* weapon);
	int GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos);
	void ResetRecoil(CUserCmd* cmd);
	void RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd);
	void DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon);

     /* weapon settings */
    bool WeaponEnabled = false;

    float WeaponFov = 0.f;
    float WeaponSmooth = 1.f;
    float WeaponRandomness = 0.f;
    bool WeaponTriggerbot = false;
    float WeaponDelay = 0.f;
    bool WeaponFlickbot = false;
    float WeaponRecoilX = 1.f;
    float WeaponRecoilY = 1.f;
    bool WeaponRcs = false;

    bool WeaponHitboxHead = false;
    bool WeaponHitboxNeck = false;
    bool WeaponHitboxChest = false;
    bool WeaponHitboxPelvis = false;
    bool WeaponHitboxStomach = false;
    bool WeaponHitboxArm = false;
    bool WeaponHitboxLeg = false;
    bool WeaponHitboxFoot = false;

	QAngle LastAimpunchRemove = QAngle(0, 0, 0);
	QAngle LastAimpunch = QAngle(0, 0, 0);
};
