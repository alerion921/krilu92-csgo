
#include "EnginePrediction.h"
#include "RuntimeSaver.hpp"
#include "helpers\math.hpp"

void EnginePrediction::run_prediction(CUserCmd* cmd)
{
    static int flTickBase;
    static CUserCmd* pLastCmd;
    auto local_player = g_LocalPlayer;
    if (!local_player)
    {
        return;
    }

    // fix tickbase if game didnt render previous tick
    if (pLastCmd)
    {
        if (pLastCmd->hasbeenpredicted)
        {
            flTickBase = local_player->m_nTickBase();
        }
        else
        {
            ++flTickBase;
        }
    }

    pLastCmd = cmd;
    flOldCurtime = g_GlobalVars->curtime;
    flOldFrametime = g_GlobalVars->frametime;
    //SDK::CGV::uRandomSeed = getRandomSeed();



    g_Saver.curtime = flTickBase * g_GlobalVars->interval_per_tick;
    //g_GlobalVars->curtime = flTickBase * g_GlobalVars->interval_per_tick;
    g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

    /*
    //INTERFACES::pMovement->StartTrackPredictionErrors(local_player);
    g_GameMovement->StartTrackPredictionErrors(local_player);

    //CMoveData

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    g_MoveHelper->SetHost(local_player);
    //g_pre
    g_Prediction->SetupMove(local_player, cmd, g_MoveHelper, &data);
    g_GameMovement->ProcessMovement(local_player, &data);
    g_Prediction->FinishMove(local_player, cmd, &data);
    */
}

void EnginePrediction::end_prediction(CUserCmd* cmd)
{
    auto local_player = g_LocalPlayer;
    if (!local_player)
    {
        return;
    }

    //g_GameMovement->FinishTrackPredictionErrors(local_player);
    //g_MoveHelper->SetHost(nullptr);

    g_GlobalVars->curtime = flOldCurtime;
    //if (!g_Saver.InFakewalk) g_GlobalVars->frametime = flOldFrametime;
}

EnginePrediction* prediction = new EnginePrediction();