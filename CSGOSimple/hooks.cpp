#include "hooks.hpp"
#include <intrin.h>  

#include "render.hpp"
#include "options.hpp"
#include "menu_helpers.hpp"
#include "ConfigSystem.h"
#include "EnginePrediction.h"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "helpers/math.hpp"
#include "features/bhop.hpp"
#include "features/nosmoke.hpp"
#include "features/aimbot.h"
#include "features/Chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "features/misc.hpp"
#include "MovementFix.h"
#include "HitPossitionHelper.h"
#include "Resolver.h"
#include "RuntimeSaver.hpp"
#include "Backtrack.h"
#include "ragebot.h"
#include "Fakelag.h"
#include "AntiAim.h"
#include "ThirdpersonAngleHelper.h"
#include "KeyLoop.h"
#include "globalvariables.h"

#pragma intrinsic(_ReturnAddress)  

namespace Hooks {

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);
		gameevents_hook.setup(g_GameEvents);
		bspquery_hook.setup(g_BSPhook);

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		gameevents_hook.hook_index(index::FireEvent, hkFireEvent);
		bspquery_hook.hook_index(index::ListLeavesInBox, hkListLeavesInBox);
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();
		gameevents_hook.unhook_all();
		bspquery_hook.unhook_all();

		Glow::Get().Shutdown();
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		if (g_Unload)
			oEndScene(pDevice);

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");

		static auto crosshair_cvar = g_CVar->FindVar("crosshair");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");

		AntiAim::Get().ResetLbyPrediction();

		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Config.GetInt("viewmodel_fov"));

		mat_ambient_light_r->SetValue(g_Config.GetFloat("mat_ambient_light_r"));
		mat_ambient_light_g->SetValue(g_Config.GetFloat("mat_ambient_light_g"));
		mat_ambient_light_b->SetValue(g_Config.GetFloat("mat_ambient_light_b"));
		crosshair_cvar->SetValue(!g_Config.GetBool("esp_crosshair"));


		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		auto esp_drawlist = Render::Get().RenderScene();

		MenuHelper::Get().Render();

		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		MenuHelper::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			MenuHelper::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number || !bSendPacket || g_Unload || !g_EngineClient || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return;

		QAngle OldViewangles = cmd->viewangles;
		float OldForwardmove = cmd->forwardmove;
		float OldSidemove = cmd->sidemove;

		KeyLoop::Get().OnCreateMove();

		prediction->run_prediction(cmd);

		bool rbot = g_Config.GetBool("rbot");

		if (g_Config.GetBool("misc_bhop"))
			Movement::Get().Bunnyhop(cmd);

		if (rbot)
			Fakelag::Get().OnCreateMove(cmd, bSendPacket);

		if (g_Config.GetBool("rbot_aa"))
			AntiAim::Get().OnCreateMove(cmd, bSendPacket);

		if (rbot)
		{
			Ragebot::Get().PrecacheShit();
			Ragebot::Get().CreateMove(cmd, bSendPacket);
		}

		if (!rbot && g_Config.GetBool("aim_enable")) {
			Aimbot::Get().OnCreateMove(cmd);

			if (g_Config.GetBool("aim_backtrack"))
				Backtrack::Get().FinishLegitBacktrack(cmd);
		}

	if (rbot && g_Config.GetBool("rbot_resolver"))
			Resolver::Get().OnCreateMove(OldViewangles);

		prediction->end_prediction(cmd);

		Misc::Get().CrouchCooldown(cmd);

		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
			g_Saver.LastShotEyePos = g_LocalPlayer->GetEyePos();

		if (g_Saver.RbotDidLastShot)
		{
			g_Saver.RbotDidLastShot = false;

			if (bSendPacket)
				bSendPacket = false;
		}
		
		AntiAim::Get().LbyBreakerPrediction(cmd, bSendPacket);

		if (g_Config.GetBool("misc_showranks") && cmd->buttons & IN_SCORE)
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);

		Math::NormalizeAngles(cmd->viewangles);

		if (g_LocalPlayer && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && g_LocalPlayer->m_nMoveType() != MOVETYPE_NOCLIP)
		{
			if (g_Config.GetBool("misc_autostrafe"))
				Movement::Get().Autostrafe(cmd, OldViewangles);
			else
				MovementFix::Get().Correct(OldViewangles, cmd, OldForwardmove, OldSidemove);
		}

		if (g_Config.GetBool("rbot_slidewalk"))
			AntiAim::Get().SlideWalk(cmd);

		Math::ClampAngles(cmd->viewangles);

		Misc::Get().removeShadows();
		Misc::Get().removeBlur();
		Misc::Get().EngineRadar();

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx; not sure if we need this
			push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		if (g_Config.GetBool("esp_noscope") && !strcmp("HudZoom", g_VGuiPanel->GetName(panel)))
			return;

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
				
			//Ignore 50% cuz it called very often
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();
		}
	}
	//--------------------------------------------------------------------------------

	int __fastcall hkListLeavesInBox(std::uintptr_t ecx, std::uintptr_t edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max) {

		static auto original_fn = bspquery_hook.get_original<decltype(&hkListLeavesInBox)>(index::ListLeavesInBox);

		static auto insert_into_tree_call_list_leaves_in_box = reinterpret_cast<std::uintptr_t>(Utils::PatternScan(GetModuleHandleA("client.dll"), "89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45"));

		if (reinterpret_cast<std::uintptr_t>(_ReturnAddress()) != insert_into_tree_call_list_leaves_in_box)
			return original_fn(ecx, edx, mins, maxs, list, list_max);

		auto info = *reinterpret_cast<RenderableInfo**>(reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress()) + 0x14);

		if (!info || !info->renderable)
			return original_fn(ecx, edx, mins, maxs, list, list_max);

		auto base_entity = info->renderable->GetIClientUnknown()->GetBaseEntity();

		if (!base_entity || !base_entity->IsPlayer())
			return original_fn(ecx, edx, mins, maxs, list, list_max);

		if (!g_Config.GetBool("chams_player_enabled") && !g_Config.GetBool("chams_player_ignorez") || !g_Config.GetBool("chams_player_ignorez_team"))
			return original_fn(ecx, edx, mins, maxs, list, list_max);

		info->flags &= ~0x100;
		info->flags2 |= 0xC0;

		constexpr float MAX_COORD_FLOAT = 16384.f;
		constexpr float MIN_COORD_FLOAT = -MAX_COORD_FLOAT;

		static Vector map_min = Vector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
		static Vector map_max = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);

		return original_fn(ecx, edx, map_min, map_max, list, list_max);
	}

	//--------------------------------------------------------------------------------

	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		Misc::Get().updateBrightness();
		Misc::Get().remove3dSky();

		if (g_LocalPlayer && g_Config.GetBool("glow_players") && !g_Unload && g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);

		//if (g_ClientState->m_nDeltaTick != -1) return  ofunc(g_CHLClient, stage);
		if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return ofunc(g_CHLClient, edx, stage);

		if (!g_Unload)
			Misc::Get().OnFrameStageNotify(stage);

		switch (stage)
		{
		case FRAME_UNDEFINED:
			break;

		case FRAME_START:
		{
			if (g_Unload)
				return;

			Misc::Get().colorWorld();

			break;
		}

		case FRAME_NET_UPDATE_START:
		{
			break;
		}

		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			break;
		}

		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			if (g_Unload)
				return;

			bool rbot = g_Config.GetBool("rbot");

			if (rbot && g_Config.GetBool("rbot_resolver"))
				Resolver::Get().OnFramestageNotify();

			NoSmoke::Get().RemoveSmoke();

			for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
			{
				auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

				if (!entity || !g_LocalPlayer || !entity->IsPlayer() || entity->IsDormant()
					|| !entity->IsAlive())
					continue;

				VarMapping_t* map = entity->VarMapping();

				if (map)
				{
					for (int j = 0; j < map->m_nInterpolatedEntries; j++)
						map->m_Entries[j].m_bNeedsToInterpolate = false;
				}
			}

			break;
		}

		case FRAME_NET_UPDATE_END:
			break;

		case FRAME_RENDER_START:
		{
			if (!g_Unload)
			{
				bool rbot = g_Config.GetBool("rbot");

				if (rbot && g_Config.GetBool("rbot_aa") && g_Config.GetBool("misc_thirdperson"))
				{
					ThirdpersonAngleHelper::Get().SetThirdpersonAngle();
					ThirdpersonAngleHelper::Get().AnimFix();
				}
				else
				{
					if (g_LocalPlayer && g_LocalPlayer->IsAlive())
						g_LocalPlayer->m_bClientSideAnimation() = true;
				}

				auto old_curtime = g_GlobalVars->curtime;
				auto old_frametime = g_GlobalVars->frametime;

				for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{
					auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

					if (!entity || !g_LocalPlayer || !entity->IsPlayer() || entity->IsDormant()
						|| !entity->IsAlive())
						continue;

					if (rbot)
					{
						entity->InvalidateBoneCache();
						ThirdpersonAngleHelper::Get().EnemyAnimationFix(entity);
					}
					else
						entity->m_bClientSideAnimation() = true;

				}

				
				g_GlobalVars->curtime = old_curtime;
				g_GlobalVars->frametime = old_frametime;
				
			}
			break;
		}

		case FRAME_RENDER_END:
			break;
		}

		ofunc(g_CHLClient, edx, stage);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return ofunc(g_ClientMode, edx, vsView);
			
		if (g_Config.GetBool("misc_novisrecoil"))
			vsView->angles -= (g_LocalPlayer->m_viewPunchAngle() + g_LocalPlayer->m_aimPunchAngle() * 2 * 0.45);
		
		if (g_EngineClient->IsInGame() && vsView)
			Visuals::Get().ThirdPerson();

		C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

		if (!weapon)
			return;

		if (weapon->IsSniper() && g_LocalPlayer && g_LocalPlayer->m_bIsScoped() && !g_Config.GetBool("misc_nozoom")) {
			return ofunc(g_ClientMode, edx, vsView);
		}

		if (g_LocalPlayer->m_bIsScoped() && weapon->IsRifle()) {
			vsView->fov = g_Config.GetInt("fov_scoped");
		} 
		else {
			vsView->fov = g_Config.GetInt("fov");
		}

		ofunc(g_ClientMode, edx, vsView);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (MenuHelper::Get().IsVisible())
		{
			g_VGuiSurface->UnlockCursor();
			return;
		}

		ofunc(g_VGuiSurface);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, std::cref(pInfo), pCustomBoneToWorld);
		}

		Chams::Get().OnDrawModelExecute(ctx, state, std::cref(pInfo), pCustomBoneToWorld);

		ofunc(_this, edx, ctx, state, std::cref(pInfo), pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	
	
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}

	bool __stdcall hkFireEvent(IGameEvent* pEvent)
	{
		static auto oFireEvent = gameevents_hook.get_original<FireEvent>(index::FireEvent);

		if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return oFireEvent(g_GameEvents, pEvent);

		Ragebot::Get().OnFireEvent(pEvent);
		Resolver::Get().OnFireEvent(pEvent);
		HitPossitionHelper::Get().OnFireEvent(pEvent);

		return oFireEvent(g_GameEvents, pEvent);
	}
}

