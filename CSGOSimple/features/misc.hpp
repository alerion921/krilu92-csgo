#pragma once

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../helpers/math.hpp"

class IMatRenderContext;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class matrix3x4_t;
class IMaterial;

class Misc : public Singleton<Misc>
{
public:

    void OnFrameStageNotify(ClientFrameStage_t stage);

	void updateBrightness();
	void colorWorld();
	void remove3dSky();
	void CrouchCooldown(CUserCmd* cmd);
	void NoFlash();
	void removeShadows();
	void removeBlur();
	void EngineRadar();
};



