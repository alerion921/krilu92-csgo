#include "misc.hpp"
#include "../ConfigSystem.h"
#include "../helpers/input.hpp"
#include "../EnginePrediction.h"

template <typename T>
static constexpr auto relativeToAbsolute(int* address) noexcept
{
    return reinterpret_cast<T>(reinterpret_cast<char*>(address + 1) + *address);
}

#define FIND_PATTERN(type, ...) \
reinterpret_cast<type>(Utils::PatternScan(__VA_ARGS__))

void Misc::OnFrameStageNotify(ClientFrameStage_t stage)
{
    switch (stage)
    {
    case FRAME_UNDEFINED:
        break;
    case FRAME_START:
        break;
    case FRAME_NET_UPDATE_START:
        break;
    case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
        break;
    case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
        break;
    case FRAME_NET_UPDATE_END:
        break;
    case FRAME_RENDER_START:
        NoFlash();
        break;
    case FRAME_RENDER_END:
        break;
    }
}


void Misc::updateBrightness()
{
    static auto brightness = g_CVar->FindVar("mat_force_tonemap_scale");
    brightness->SetValue(g_Config.GetFloat("misc_brightness"));
}

void Misc::remove3dSky()
{
    static auto sky = g_CVar->FindVar("r_3dsky");
    sky->SetValue(!g_Config.GetBool("misc_nosky"));
}

void Misc::colorWorld()
{
    if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
        return;

    auto local_player = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

    if (!local_player)
        return;

    static auto r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
    r_DrawSpecificStaticProp->m_nFlags &= ~FCVAR_CHEAT;
    r_DrawSpecificStaticProp->SetValue(1);

    static auto sv_skyname = g_CVar->FindVar("sv_skyname");
    sv_skyname->m_nFlags &= ~FCVAR_CHEAT;

    static auto mat_postprocess_enable = g_CVar->FindVar("mat_postprocess_enable");
    mat_postprocess_enable->SetValue(0);

    for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
    {
        IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

        if (!pMaterial)
            continue;

        if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp")) {
            if (g_Config.GetBool("misc_world")) {
                pMaterial->ColorModulate(g_Config.GetColor("color_world")[0] / 255.f, g_Config.GetColor("color_world")[1] / 255.f, g_Config.GetColor("color_world")[2] / 255.f);
                pMaterial->AlphaModulate(1);
            }
            else {
                pMaterial->ColorModulate(1, 1, 1);
            }
        }

        if (strstr(pMaterial->GetTextureGroupName(), "SkyBox")) {
            if (g_Config.GetBool("misc_nosky_color")) {
                pMaterial->ColorModulate(g_Config.GetColor("color_sky")[0] / 255.f, g_Config.GetColor("color_sky")[1] / 255.f, g_Config.GetColor("color_sky")[2] / 255.f);
                pMaterial->AlphaModulate(1);
            }
            else {
                pMaterial->ColorModulate(1, 1, 1);
            }
        }

        if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures")) {
            if (g_Config.GetBool("misc_world_alpha")) {
                pMaterial->AlphaModulate(0.9);
            }
            else {
                pMaterial->AlphaModulate(1);
            }
            if (g_Config.GetBool("misc_color_props")) {
                pMaterial->ColorModulate(g_Config.GetColor("color_props")[0] / 255.f, g_Config.GetColor("color_props")[1] / 255.f, g_Config.GetColor("color_props")[2] / 255.f);
            }
            else {
                pMaterial->ColorModulate(1, 1, 1);
            }
        }
    }
}

void Misc::CrouchCooldown(CUserCmd* cmd)
{
    if (!g_Config.GetBool("misc_crouchcooldown"))
    {
        return;
    }

    cmd->buttons |= IN_BULLRUSH;
}

void Misc::NoFlash()
{
    if (!g_Config.GetBool("misc_noflash"))
        return;

    if (!g_LocalPlayer)
        return;

    g_LocalPlayer->m_flFlashMaxAlpha() = 0.f;
}

void Misc::removeShadows()
{
    static auto shadows = g_CVar->FindVar("cl_csm_enabled");
    shadows->SetValue(!g_Config.GetBool("misc_noshadows"));
}

void Misc::removeBlur()
{
    static auto blur = g_MatSystem->FindMaterial("dev/scope_bluroverlay");
    blur->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, g_Config.GetBool("misc_noblur"));
}

void Misc::EngineRadar() {

    if (!g_Config.GetBool("misc_engine_radar"))
        return;

    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
        if (!entity || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
        || !entity->IsAlive() || !entity->IsEnemy())
        {
            continue;
        }
        
        entity->m_bSpotted() = true;
    }
}