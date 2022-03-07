
#include "ThirdpersonAngleHelper.h"
#include "RuntimeSaver.hpp"
#include "Resolver.h"

void ThirdpersonAngleHelper::SetThirdpersonAngle()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    g_LocalPlayer->SetVAngles ( g_Saver.FakelagData.ang );
    LastAngle = g_Saver.FakelagData.ang;

    g_Saver.HasChokedLast = g_ClientState->m_nChokedCommands >= 1;
    g_Saver.LastRealAnglesBChoke = g_Saver.AARealAngle;
}

void ThirdpersonAngleHelper::EnemyAnimationFix ( C_BasePlayer* player )
{
    AnimFix2 ( player ); //xD
}

void ThirdpersonAngleHelper::AnimFix()
{
}

void ThirdpersonAngleHelper::AnimFix2 ( C_BasePlayer* entity )
{
}