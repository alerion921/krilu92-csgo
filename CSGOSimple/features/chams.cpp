#include "chams.hpp"
#include "../ConfigSystem.h"
#include "../Backtrack.h"
#include "../helpers/math.hpp"

Chams::Chams() {

	std::ofstream{ "csgo/materials/chamsNormal.vmt" } <<
		"VertexLitGeneric { }";

	std::ofstream{ "csgo/materials/chamsFlat.vmt" } <<
		"UnlitGeneric { }";

	std::ofstream{ "csgo/materials/chamsAnimated.vmt" } <<
		"VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint \"[.7 .7 .7]\" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } }";

	std::ofstream("csgo/materials/glowOverlay.vmt") <<
		"VertexLitGeneric { $additive 1 $envmap models/effects/cube_white $envmaptint \"[1 0 0]\" $envmapfresnel 1 $envmapfresnelminmaxexp \"[0 1 2]\" $alpha 0.8 }";

	normalNew = g_MatSystem->FindMaterial("chamsNormal");
	normalNew->IncrementReferenceCount();
	flatNew = g_MatSystem->FindMaterial("chamsFlat");
	flatNew->IncrementReferenceCount();
	animatedNew = g_MatSystem->FindMaterial("chamsAnimated");
	animatedNew->IncrementReferenceCount();
	platinumNew = g_MatSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass");
	platinumNew->IncrementReferenceCount();
	glassNew = g_MatSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass");
	glassNew->IncrementReferenceCount();
	crystalNew = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear");
	crystalNew->IncrementReferenceCount();
	chromeNew = g_MatSystem->FindMaterial("models/gibs/glass/glass");
	chromeNew->IncrementReferenceCount();
	silverNew = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/silver_plain");
	silverNew->IncrementReferenceCount();
	goldNew = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gold");
	goldNew->IncrementReferenceCount();
	plasticNew = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss");
	plasticNew->IncrementReferenceCount();
	glowNew = g_MatSystem->FindMaterial("glowOverlay");
	glowNew->IncrementReferenceCount();
}

Chams::~Chams() {
}

void Chams::OverrideMaterial(bool ignoreZ, bool wireframe, int style_chams, const Color& rgba) {
	IMaterial* material = nullptr;


	switch (style_chams) {
	case 0:
		material = normalNew;
		break;
	case 1:
		material = flatNew;
		break;
	case 2:
		material = glassNew;
		break;
	case 3:
		material = animatedNew;
		break;
	case 4:
		material = platinumNew;
		break;
	case 5:
		material = crystalNew;
		break;
	case 6:
		material = chromeNew;
		break;
	case 7:
		material = silverNew;
		break;
	case 8:
		material = goldNew;
		break;
	case 9:
		material = plasticNew;
		break;
	case 10:
		material = glowNew;
		break;
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);
	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);

	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g_MdlRender->ForcedMaterialOverride(material);
}


void Chams::OnDrawModelExecute(
	IMatRenderContext* ctx,
	const DrawModelState_t& state,
	const ModelRenderInfo_t& info,
	matrix3x4_t* matrix)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);

	const auto mdl = info.pModel;

	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	//bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;

	if (is_player && g_Config.GetBool("chams_player_enabled")) {
		// 
		// Draw player Chams.
		// 
		auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);

		if (ent == g_LocalPlayer)
			return;

		if (ent && g_LocalPlayer && ent->IsAlive()) {
			const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();

			if (!enemy && !g_Config.GetBool("chams_player_enemies_only") )
				return;

			const auto ignorez_color = enemy ? g_Config.GetColor("color_chams_player_enemy_visible") : g_Config.GetColor("color_chams_player_ally_visible");
			const auto chams_color = enemy ? g_Config.GetColor("color_chams_player_enemy_occluded") : g_Config.GetColor("color_chams_player_ally_occluded");
			const auto ignorez_style = enemy ? g_Config.GetInt("chams_player_ignorez_style") : g_Config.GetInt("chams_player_ignorez_style_team");
			const auto chams_style = enemy ? g_Config.GetInt("chams_player_style") : g_Config.GetInt("chams_player_style_team");
			const auto wireframe = enemy ? g_Config.GetBool("chams_player_wireframe") : g_Config.GetBool("chams_player_wireframe_team");

			if (g_Config.GetBool("chams_player_ignorez") || g_Config.GetBool("chams_player_ignorez_team")) {
				OverrideMaterial(true, wireframe, ignorez_style, ignorez_color);
				fnDME(g_MdlRender, 0, ctx, state, std::cref(info), matrix);
				OverrideMaterial(false, wireframe, chams_style, chams_color);
			}
			else {
				OverrideMaterial(false, wireframe, chams_style, chams_color);
			}
		}
		if (g_Config.GetBool("aim_backtrack")) {
		}
	}
	else if (is_sleeve && g_Config.GetBool("chams_arms_enabled")) {
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;

		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		g_MdlRender->ForcedMaterialOverride(material);
	}
	else if (is_arm) {
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;
		if (g_Config.GetBool("misc_nohands")) {
			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			g_MdlRender->ForcedMaterialOverride(material);
		}
		else if (g_Config.GetBool("chams_arms_enabled")) {
			if (g_Config.GetBool("chams_arms_ignorez")) {
				OverrideMaterial(
					true,
					g_Config.GetBool("chams_arms_wireframe"),
					g_Config.GetInt("chams_arms_ignorez_style"),
					g_Config.GetColor("color_chams_arms_occluded"));
				fnDME(g_MdlRender, 0, ctx, state, std::cref(info), matrix);
				OverrideMaterial(
					false,
					g_Config.GetBool("chams_arms_wireframe"),
					g_Config.GetInt("chams_arms_style"),
					g_Config.GetColor("color_chams_arms_visible"));
			}
			else {
				OverrideMaterial(
					false,
					g_Config.GetBool("chams_arms_wireframe"),
					g_Config.GetInt("chams_arms_style"),
					g_Config.GetColor("color_chams_arms_visible"));
			}
		}
	}
}