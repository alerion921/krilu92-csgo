
#include "valve_sdk/csgostructs.hpp"

#pragma once
class EnginePrediction
{
public:
    void run_prediction(CUserCmd* cmd);
    void end_prediction(CUserCmd* cmd);
private:
    float flOldCurtime = 0.f;
    float flOldFrametime = 0.f;
};

extern EnginePrediction* prediction;