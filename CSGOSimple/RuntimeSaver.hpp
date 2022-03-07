
#include "valve_sdk/csgostructs.hpp"

#pragma once

struct FakeLagData
{
    Vector pos = Vector(0, 0, 0);
    QAngle ang = QAngle(0, 0, 0);
};

struct ImpactInfoStruct
{
    Vector ShotPos = Vector(0, 0, 0);
    Vector Pos = Vector(0, 0, 0);
    float Time = 0.f;
    int Tick = 0;
};

struct BulletTracerInfoStruct
{
    bool DidHit = false;
    float Time = 0.f;
    Vector ShotPos = Vector(0, 0, 0);
    Vector HitPos = Vector(0, 0, 0);
};

struct HitmarkerInfoStruct
{
    float HitTime = 0.f;
    float Damage = 0.f;
};

struct RbotShotInfoStruct
{
    bool InLc = false;
    bool InLbyUpdate = false;
    bool Moving = false;
};

class RuntimeSaver
{
public:
    QAngle AARealAngle = QAngle(0, 0, 0);
    QAngle AAFakeAngle = QAngle(0, 0, 0);
    bool FakelagCurrentlyEnabled = false;
    FakeLagData FakelagData;
    float curtime = 0.f;

    //Lby update
    bool InLbyUpdate = false;
    float NextLbyUpdate = 0.f;

    //AnimationFixData
    bool HasChokedLast = false;
    QAngle LastRealAnglesBChoke = QAngle(0, 0, 0);

    //HitposHelper
    std::vector<ImpactInfoStruct> hitpositions;

    //BulletTracers
    std::vector<BulletTracerInfoStruct> BulletTracers;

    //Rbot
    bool RbotAADidShot = false;

    bool CurrentInLbyBreak = false;
    bool CurrentShouldSkipAnimations = false;

    //Fakewalk
    bool InFakewalk = false;

    int LastBacktrackTick = -1;

    bool LCbroken = false;

    HitmarkerInfoStruct HitmarkerInfo;

    RbotShotInfoStruct RbotShotInfo;

    Vector LastShotEyePos = Vector(0, 0, 0);

    bool RbotDidLastShot = false;
};

extern RuntimeSaver g_Saver;