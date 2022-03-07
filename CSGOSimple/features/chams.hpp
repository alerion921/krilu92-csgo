#pragma once

#include "../singleton.hpp"
#include <fstream>
#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"

class IMatRenderContext;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class matrix3x4_t;
class IMaterial;
class Color;

class Chams
    : public Singleton<Chams>
{
    friend class Singleton<Chams>;

    Chams();
    ~Chams();

public:
	void OnDrawModelExecute(
        IMatRenderContext* ctx,
        const DrawModelState_t &state,
        const ModelRenderInfo_t &pInfo,
        matrix3x4_t *pCustomBoneToWorld);

private:
    void OverrideMaterial(bool ignoreZ, bool wireframe, int style_chams, const Color& rgba);

    IMaterial* normalNew = nullptr;
    IMaterial* flatNew = nullptr;
    IMaterial* animatedNew = nullptr;
    IMaterial* platinumNew = nullptr;
    IMaterial* glassNew = nullptr;
    IMaterial* crystalNew = nullptr;
    IMaterial* chromeNew = nullptr;
    IMaterial* silverNew = nullptr;
    IMaterial* goldNew = nullptr;
    IMaterial* plasticNew = nullptr;
    IMaterial* glowNew = nullptr;
};