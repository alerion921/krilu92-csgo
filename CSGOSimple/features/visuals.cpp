
#include "visuals.hpp"
#include "../Resolver.h"
#include "../RuntimeSaver.hpp"
#include "../Autowall.h"
#include "../menu.hpp"
#include "../ConfigSystem.h"

std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

Visuals::Visuals()
{
	InitializeCriticalSection(&cs);
}

Visuals::~Visuals() {
	DeleteCriticalSection(&cs);
}

//--------------------------------------------------------------------------------
void Visuals::Render() {
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	if (pl->IsDormant() || !pl->IsAlive())
		return false;

	ctx.pl = pl;
	ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!ctx.is_enemy && !g_Config.GetBool("esp_enemies_only"))
		return false;

	ctx.clr = ctx.is_enemy ? (ctx.is_visible ? g_Config.GetColor("color_esp_enemy_visible") : g_Config.GetColor("color_esp_enemy_occluded")) : (ctx.is_visible ? g_Config.GetColor("color_esp_ally_visible") : g_Config.GetColor("color_esp_ally_occluded"));

	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, ctx.head_pos) ||
		!Math::WorldToScreen(origin, ctx.feet_pos))
		return false;

	auto h = fabs(ctx.head_pos.y - ctx.feet_pos.y);
	auto w = h / 1.65f;

	ctx.bbox.left = static_cast<long>(ctx.feet_pos.x - w * 0.5f);
	ctx.bbox.right = static_cast<long>(ctx.bbox.left + w);
	ctx.bbox.bottom = static_cast<long>(ctx.feet_pos.y);
	ctx.bbox.top = static_cast<long>(ctx.head_pos.y);

	return true;
}
//--------------------------------------------------------------------------------

void Visuals::Player::RenderBox() {
	if (g_Config.GetInt("esp_boxtype") == 0 || g_Config.GetInt("esp_boxtype") == 1) {
		Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1.0f, g_Config.GetInt("esp_boxtype"));
	}
	else if (g_Config.GetInt("esp_boxtype") == 2) {
		int screen_w, screen_h;
		g_EngineClient->GetScreenSize(screen_w, screen_h);

		ICollideable* coll = ctx.pl->GetCollideable();

		Vector min = coll->OBBMins();
		Vector max = coll->OBBMaxs();

		Vector corners[8] =
		{
			Vector(min.x,min.y,min.z),
			Vector(min.x,max.y,min.z),
			Vector(max.x,max.y,min.z),
			Vector(max.x,min.y,min.z),
			Vector(min.x,min.y,max.z),
			Vector(min.x,max.y,max.z),
			Vector(max.x,max.y,max.z),
			Vector(max.x,min.y,max.z)
		};

		float ang = ctx.pl->m_angEyeAngles().yaw;

		for (int i = 0; i <= 7; i++)
			corners[i].Rotate2D(ang);

		Vector _corners[8];

		for (int i = 0; i <= 7; i++)
			Math::WorldToScreen(ctx.pl->m_vecOrigin() + corners[i], _corners[i]);

		int x1 = screen_w * 2, y1 = screen_h * 2, x2 = -screen_w, y2 = -screen_h;

		for (int i = 0; i <= 7; i++)
		{
			x1 = (int)(min(x1, _corners[i].x));
			y1 = (int)(min(y1, _corners[i].y));
			x2 = (int)(max(x2, _corners[i].x));
			y2 = (int)(max(y2, _corners[i].y));
		}

		int diff = x2 - x1;
		int diff2 = y2 - y1;

		for (int i = 1; i <= 4; i++)
		{
			Render::Get().RenderLine((int)(_corners[i - 1].x), (int)(_corners[i - 1].y), (int)(_corners[i % 4].x), (int)(_corners[i % 4].y), ctx.clr);
			Render::Get().RenderLine((int)(_corners[i - 1].x), (int)(_corners[i - 1].y), (int)(_corners[i + 3].x), (int)(_corners[i + 3].y), ctx.clr);
			Render::Get().RenderLine((int)(_corners[i + 3].x), (int)(_corners[i + 3].y), (int)(_corners[i % 4 + 4].x), (int)(_corners[i % 4 + 4].y), ctx.clr);
		}
	}
}

//--------------------------------------------------------------------------------
void Visuals::Player::RenderName()
{
	player_info_t info = ctx.pl->GetPlayerInfo();

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, info.szName);

	Render::Get().RenderText(info.szName, ctx.feet_pos.x - sz.x / 2, ctx.head_pos.y - sz.y, 14.f,  ctx.clr);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{
	auto  hp = ctx.pl->m_iHealth();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	float off = 8;

	int height = (box_h * hp) / 100;

	int green = int(hp * 2.55f);
	int red = 255 - green;

	int x = ctx.bbox.left - off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBox(x, y, x + w, y + h, Color::Black, 1.f, true);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(red, green, 0, 255), 1.f, true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	int height = (((box_h * armour) / 100));

	int x = ctx.bbox.right + off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBox(x, y, x + w, y + h, Color::Black, 1.f, true);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(0, 50, 255, 255), 1.f, true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeaponName()
{
	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;

	auto text = weapon->GetCSWeaponData()->szWeaponName + 7;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, text);

	Render::Get().RenderText(text, ctx.feet_pos.x, ctx.feet_pos.y, 14.f, ctx.pl->IsDormant() ? ctx.clr : g_Config.GetColor("color_esp_weapons"));
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderAmmo()
{
	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;

	auto text = weapon->GetCSWeaponData()->szWeaponName + 7;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, text);

	std::string ammo = string_format("[%i/%i]", weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount());
	auto sz2 = g_pDefaultFont->CalcTextSizeA(15.f, FLT_MAX, 0.0f, ammo.c_str());

	Render::Get().RenderText(ammo, ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left) * 0.5f - sz2.x * 0.5f, ctx.bbox.bottom + 1.f + sz2.y, 14.f, ctx.pl->IsDormant() ? ctx.clr : g_Config.GetColor("color_esp_ammo"));
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{

	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	Render::Get().RenderLine(screen_w / 2.f, (float)screen_h,
		ctx.feet_pos.x, ctx.feet_pos.y, ctx.clr);
}

void Visuals::Player::RenderSkeleton() {

	studiohdr_t* p_studio_hdr = g_MdlInfo->GetStudiomodel(ctx.pl->GetModel());

	if (!p_studio_hdr)
		return;

	Vector v_parent, v_child, s_parent, s_child, v_head;

	for (int i = 0; i < p_studio_hdr->numbones; i++) {
		mstudiobone_t* bone = p_studio_hdr->GetBone(i);

		if (!bone)
			return;

		if (bone && (bone->flags & BONE_USED_BY_HITBOX) && (bone->parent != -1)) {
			v_child = ctx.pl->GetBonePos(i);
			v_parent = ctx.pl->GetBonePos(bone->parent);

			if (Math::WorldToScreen(v_parent, s_parent) && Math::WorldToScreen(v_child, s_child)) {
				Render::Get().RenderLine(s_parent[0], s_parent[1], s_child[0], s_child[1], g_Config.GetColor("color_esp_skeleton"));
			}
			Vector MyEyePos = g_LocalPlayer->GetEyePos();

			if (i == 8) {
				if (Math::WorldToScreen(v_parent, s_parent) && Math::WorldToScreen(v_child, s_child) && g_Config.GetBool("esp_skeleton_head"))
					Render::Get().RenderCircle(s_child[0], s_child[1], 4500 / Math::VectorDistance(MyEyePos, v_child), 35, g_Config.GetColor("color_esp_skeleton_head"));
			}
		}
	}
}

void Visuals::Player::DrawPlayerDebugInfo()
{
	if (!g_LocalPlayer || ctx.pl == g_LocalPlayer)
		return;

	if (!ctx.pl->IsEnemy())
		return;

	std::string t1 = "Missed shots: " + std::to_string(g_Resolver.GResolverData[ctx.pl->EntIndex()].Shots);
	std::string t2 = "Mode: ";
	std::string t3 = "Detected: ";
	std::string t4 = g_Resolver.GResolverData[ctx.pl->EntIndex()].Fake ? "fake" : "real";
	std::string t5 = "Velocity: " + std::to_string(ctx.pl->m_vecVelocity().Length2D());
	int i = ctx.pl->EntIndex();

	switch (g_Resolver.GResolverData[i].mode)
	{
	case ResolverModes::NONE:
		t2 += "none";
		break;

	case ResolverModes::FREESTANDING:
		t2 += "FREESTANDING";
		break;

	case ResolverModes::EDGE:
		t2 += "EDGE";
		break;

	case ResolverModes::MOVE_STAND_DELTA:
		t2 += "MOVE_STAND_DELTA";
		break;

	case ResolverModes::FORCE_LAST_MOVING_LBY:
		t2 += "FORCE_LAST_MOVING_LBY";
		break;

	case ResolverModes::FORCE_FREESTANDING:
		t2 += "FORCE_FREESTANDING";
		break;

	case ResolverModes::BRUTFORCE_ALL_DISABLED:
		t2 += "BRUTFORCE_ALL_DISABLED";
		break;

	case ResolverModes::BRUTFORCE:
		t2 += "BRUTFORCE";
		break;

	case ResolverModes::FORCE_MOVE_STAND_DELTA:
		t2 += "FORCE_MOVE_STAND_DELTA";
		break;

	case ResolverModes::FORCE_LBY:
		t2 += "FORCE_LBY";
		break;

	case ResolverModes::MOVING:
		t2 += "MOVING";
		break;

	case ResolverModes::LBY_BREAK:
		t2 += "LBY_BREAK";
		break;

	case ResolverModes::SPINBOT:
		t2 += "SPINBOT";
		break;

	case ResolverModes::AIR_FREESTANDING:
		t2 += "AIR_FREESTANDING";
		break;

	case ResolverModes::AIR_BRUTFORCE:
		t2 += "AIR_BRUTFORCE";
		break;

	case ResolverModes::FAKEWALK_FREESTANDING:
		t2 += "FAKEWALK_FREESTANDING";
		break;

	case ResolverModes::FAKEWALK_BRUTFORCE:
		t2 += "FAKEWALK_BRUTFORCE";
		break;

	case ResolverModes::BACKWARDS:
		t2 += "BACKWARDS";
		break;

	case ResolverModes::FORCE_BACKWARDS:
		t2 += "FORCE_BACKWARDS";
		break;
	}

	switch (g_Resolver.GResolverData[i].detection)
	{
	case ResolverDetections::FAKEWALKING:
		t3 += "Fakewalking";
		break;

	case ResolverDetections::AIR:
		t3 += "Air";
		break;

	case ResolverDetections::MOVING:
		t3 += "Moving";
		break;

	case ResolverDetections::STANDING:
		t3 += "Standing";
		break;
	}

	Render::Get().RenderText(t1, ctx.bbox.right + 48.f, ctx.head_pos.y, 14.f, ctx.clr, true, g_pDefaultFont);
	Render::Get().RenderText(t2, ctx.bbox.right + 48.f, ctx.head_pos.y + 14.f, 14.f, ctx.clr, true, g_pDefaultFont);
	Render::Get().RenderText(t3, ctx.bbox.right + 48.f, ctx.head_pos.y + 28.f, 14.f, ctx.clr, true, g_pDefaultFont);
	Render::Get().RenderText(t4, ctx.bbox.right + 48.f, ctx.head_pos.y + 42.f, 14.f, ctx.clr, true, g_pDefaultFont);
	Render::Get().RenderText(t5, ctx.bbox.right + 48.f, ctx.head_pos.y + 56.f, 14.f, ctx.clr, true, g_pDefaultFont);
}
//--------------------------------------------------------------------------------
void Visuals::RenderCrosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	int cx = w / 2;
	int cy = h / 2;
	Render::Get().RenderLine(cx - 25, cy, cx + 25, cy, g_Config.GetColor("color_esp_crosshair"));
	Render::Get().RenderLine(cx, cy - 25, cx, cy + 25, g_Config.GetColor("color_esp_crosshair"));
}
//--------------------------------------------------------------------------------
void Visuals::RenderWeapon(C_BaseCombatWeapon* ent)
{
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, g_Config.GetColor("color_esp_dropped_weapons"));


	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;


	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Config.GetColor("color_esp_dropped_weapons"));
}
//--------------------------------------------------------------------------------
void Visuals::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, g_Config.GetColor("color_esp_defuse"));

	auto name = "Defuse Kit";
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Config.GetColor("color_esp_defuse"));
}
//--------------------------------------------------------------------------------
void Visuals::RenderPlantedC4(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	Render::Get().RenderBox(bbox, g_Config.GetColor("color_esp_c4"));


	int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = std::to_string(bombTimer);

	auto name = (bombTimer < 0.f) ? "Bomb" : timer;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name.c_str());
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Config.GetColor("color_esp_c4"));
}
//--------------------------------------------------------------------------------
void Visuals::RenderItemEsp(C_BaseEntity* ent)
{
	std::string itemstr = "Undefined";
	const model_t * itemModel = ent->GetModel();
	if (!itemModel)
		return;
	studiohdr_t * hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr)
		return;
	itemstr = hdr->szName;
	if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
		itemstr = "";
	else if (itemstr.find("case_pistol") != std::string::npos)
		itemstr = "Pistol Case";
	else if (itemstr.find("case_light_weapon") != std::string::npos)
		itemstr = "Light Case";
	else if (itemstr.find("case_heavy_weapon") != std::string::npos)
		itemstr = "Heavy Case";
	else if (itemstr.find("case_explosive") != std::string::npos)
		itemstr = "Explosive Case";
	else if (itemstr.find("case_tools") != std::string::npos)
		itemstr = "Tools Case";
	else if (itemstr.find("random") != std::string::npos)
		itemstr = "Airdrop";
	else if (itemstr.find("dz_armor_helmet") != std::string::npos)
		itemstr = "Full Armor";
	else if (itemstr.find("dz_helmet") != std::string::npos)
		itemstr = "Helmet";
	else if (itemstr.find("dz_armor") != std::string::npos)
		itemstr = "Armor";
	else if (itemstr.find("upgrade_tablet") != std::string::npos)
		itemstr = "Tablet Upgrade";
	else if (itemstr.find("briefcase") != std::string::npos)
		itemstr = "Briefcase";
	else if (itemstr.find("parachutepack") != std::string::npos)
		itemstr = "Parachute";
	else if (itemstr.find("dufflebag") != std::string::npos)
		itemstr = "Cash Dufflebag";
	else if (itemstr.find("ammobox") != std::string::npos)
		itemstr = "Ammobox";
	else if (itemstr.find("dronegun") != std::string::npos)
		itemstr = "Turrel";
	else if (itemstr.find("exojump") != std::string::npos)
		itemstr = "Exojump";
	else if (itemstr.find("healthshot") != std::string::npos)
		itemstr = "Healthshot";
	else {
		return;
	}
	
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;


	Render::Get().RenderBox(bbox, g_Config.GetColor("color_esp_items"));
	Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Config.GetColor("color_esp_items"));
}
//--------------------------------------------------------------------------------

void Visuals::FOVCircle()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	int x, y;
	g_EngineClient->GetScreenSize(x, y);
	float cx = x / 2.f;
	float cy = y / 2.f;

	float aimbotFov;

	if (weapon->IsPistol())
	{
		aimbotFov = g_Config.GetFloat("aim_pistol_fov");
	}
	else if (weapon->IsSniper())
	{
		aimbotFov = g_Config.GetFloat("aim_sniper_fov");
	}
	else if (weapon->IsSubmachinegun())
	{
		aimbotFov = g_Config.GetFloat("aim_smg_fov");
	}
	else if (weapon->IsMachinegun())
	{
		aimbotFov = g_Config.GetFloat("aim_mg_fov");
	}
	else if (weapon->IsShotgun())
	{
		aimbotFov = g_Config.GetFloat("aim_shotgun_fov");
	}
	else
	{
		aimbotFov = g_Config.GetFloat("aim_rifle_fov");
	}

	int fov = g_Config.GetInt("fov");
	float radius = tanf(DEG2RAD(aimbotFov) / 2) / tanf(DEG2RAD(fov) / 2) * cx;

	Render::Get().RenderCircle(cx, cy, radius, 35, g_Config.GetColor("color_esp_fovcircle"));
}

void Visuals::SpreadCircle()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	float spread = weapon->GetInaccuracy() * 1000;

	if (spread == 0.f)
		return;

	int x, y;
	g_EngineClient->GetScreenSize(x, y);
	float cx = x / 2.f;
	float cy = y / 2.f;

	Render::Get().RenderCircle(cx, cy, spread, 35, g_Config.GetColor("color_esp_spreadcircle"));
}


void Visuals::Hitmarker()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	static int cx;
	static int cy;
	static int w, h;

	g_EngineClient->GetScreenSize(w, h);
	cx = w / 2;
	cy = h / 2;

	if (g_GlobalVars->realtime - g_Saver.HitmarkerInfo.HitTime > .5f)
		return;

	float percent = (g_GlobalVars->realtime - g_Saver.HitmarkerInfo.HitTime) / .5f;
	float percent2 = percent;

	if (percent > 1.f)
	{
		percent = 1.f;
		percent2 = 1.f;
	}

	percent = 1.f - percent;
	float addsize = percent2 * 5.f;


	Color clr = Color(255, 255, 255, (int)(percent * 255.f));

	Render::Get().RenderLine(cx - 3.f - addsize, cy - 3.f - addsize, cx + 3.f + addsize, cy + 3.f + addsize, g_Config.GetColor("color_esp_hitmarker"), 1.f);
	Render::Get().RenderLine(cx - 3.f - addsize, cy + 3.f + addsize, cx + 3.f + addsize, cy - 3.f - addsize, g_Config.GetColor("color_esp_hitmarker"), 1.f);
}

void Visuals::AutoWallCrosshair()
{

	   if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;
	   float Damage = 0.f;
	   Autowall::Get().trace_awall(Damage);
	   if (Damage != 0.f)
	   {
		   int x, y;
		   g_EngineClient->GetScreenSize(x, y);

		   float cx = x / 2.f, cy = y / 2.f;

		   Render::Get().RenderText("Damage: " + std::to_string(Damage), cx, cy, 14.f, g_Config.GetColor("color_esp_autowall"));
	   }
}

void Visuals::RecoilCrosshair()
{
	auto local = g_LocalPlayer;
	if (!g_LocalPlayer)
		return;

	QAngle ViewAngles;
	g_EngineClient->GetViewAngles(ViewAngles);
	ViewAngles += (local->m_aimPunchAngle()) * 2.f;

	Vector fowardVec;
	Math::AngleVectors(ViewAngles, fowardVec);
	fowardVec *= 10000;

	Vector start = local->GetEyePos();
	Vector end = start + fowardVec, endScreen;

	if (Math::WorldToScreen(end, endScreen) && local->IsAlive())
	{
		Render::Get().RenderLine(endScreen.x - 10, endScreen.y, endScreen.x + 10, endScreen.y, g_Config.GetColor("color_esp_recoil_cross"));
		Render::Get().RenderLine(endScreen.x, endScreen.y - 10, endScreen.x, endScreen.y + 10, g_Config.GetColor("color_esp_recoil_cross"));
	}
}

void Visuals::ThirdPerson() {
	if (!g_LocalPlayer)
		return;

	if (g_Config.GetBool("misc_thirdperson") && g_LocalPlayer->IsAlive())
	{
		if (!g_Input->m_fCameraInThirdPerson)
		{
			g_Input->m_fCameraInThirdPerson = true;
		}

		float dist = g_Config.GetFloat("misc_thirdperson_dist");

		QAngle *view = g_LocalPlayer->GetVAngles();
		trace_t tr;
		Ray_t ray;

		Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(-view->pitch)) * dist
		);

		ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
		CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

		float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK)
		{
			cameraDistance = dist;
		}
		else
		{
			if (vertOK)
			{
				cameraDistance = distance2D * 0.95f;
			}
			else
			{
				cameraDistance = abs(diff.z) * 0.95f;
			}
		}
		g_Input->m_fCameraInThirdPerson = true;

		g_Input->m_vecCameraOffset.z = cameraDistance;
	}
	else
	{
		g_Input->m_fCameraInThirdPerson = false;
	}
}

void Visuals::NoScopeOverlay()
{
	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_EngineClient->IsInGame())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (!weapon->IsSniper())
		return;


	static int cx;
	static int cy;
	static int w, h;

	g_EngineClient->GetScreenSize(w, h);
	cx = w / 2;
	cy = h / 2;

	if (g_LocalPlayer->m_bIsScoped())
	{
		Render::Get().RenderLine(0, cy, w, cy, Color::Black);
		Render::Get().RenderLine(cx, 0, cx, h, Color::Black);
	}
}

void Visuals::DrawGrenade(C_BaseEntity* ent)
{
	ClassId id = ent->GetClientClass()->m_ClassID;
	Vector vGrenadePos2D;
	Vector vGrenadePos3D = ent->m_vecOrigin();

	if (!Math::WorldToScreen(vGrenadePos3D, vGrenadePos2D))
		return;

	switch (id)
	{
	case ClassId::ClassId_CSmokeGrenadeProjectile:
		Render::Get().RenderText("smoke", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
		break;

	case ClassId::ClassId_CBaseCSGrenadeProjectile:
	{
		const model_t* model = ent->GetModel();

		if (!model)
		{
			Render::Get().RenderText("nade", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
			return;
		}

		studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);

		if (!hdr)
		{
			Render::Get().RenderText("nade", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
			return;
		}

		std::string name = hdr->szName;

		if (name.find("incendiarygrenade") != std::string::npos || name.find("fraggrenade") != std::string::npos)
		{
			Render::Get().RenderText("frag", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
			return;
		}

		Render::Get().RenderText("flash", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
		break;
	}

	case ClassId::ClassId_CMolotovProjectile:
		Render::Get().RenderText("molo", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
		break;

	case ClassId::ClassId_CDecoyProjectile:
		Render::Get().RenderText("decoy", vGrenadePos2D.x, vGrenadePos2D.y, 12.f, g_Config.GetColor("color_esp_grenade"));
		break;
	}
}

void Visuals::GrenadePrediction()
{
	/* Check if we are connected */
	if (!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame())
		return;

	/* Check if we are alive */
	if (!g_LocalPlayer->IsAlive())
		return;

	/* If the grenade prediction isn't enabled, return */
	if (!g_Config.GetBool("esp_grenade_prediction"))
		return;

	/* Pointer to the local player */
	auto p_local = g_LocalPlayer;

	/* If the local player is null, don't do anything */
	if (!p_local)
		return;

	/* Pointer to the local player active weapon */
	auto weapon = p_local->m_hActiveWeapon();

	/* If we enable the grenade prediction */
	if (g_Config.GetBool("esp_grenade_prediction"))
	{
		const float TIMEALIVE = 5.f;
		const float GRENADE_COEFFICIENT_OF_RESTITUTION = 0.4f;

		float fStep = 0.1f;
		float fGravity = 800.0f / 8.f;

		Vector vPos;
		Vector vStart;
		Vector vThrow;
		Vector vThrow2;
		int iCollisions = 0;

		QAngle vViewAngles;
		g_EngineClient->GetViewAngles(vViewAngles);

		vThrow[0] = vViewAngles[0];
		vThrow[1] = vViewAngles[1];
		vThrow[2] = vViewAngles[2];

		if (vThrow[0] < 0)
			vThrow[0] = -10 + vThrow[0] * ((90 - 10) / 90.0);
		else
			vThrow[0] = -10 + vThrow[0] * ((90 + 10) / 90.0);

		float fVel = (90 - vThrow[0]) * 4;
		if (fVel > 500)
			fVel = 500;

		Math::AngleVectors1234(vThrow, &vThrow2);

		Vector vEye = p_local->GetEyePos();
		vStart[0] = vEye[0] + vThrow2[0] * 16;
		vStart[1] = vEye[1] + vThrow2[1] * 16;
		vStart[2] = vEye[2] + vThrow2[2] * 16;

		vThrow2[0] = (vThrow2[0] * fVel) + p_local->m_vecVelocity()[0];
		vThrow2[1] = (vThrow2[1] * fVel) + p_local->m_vecVelocity()[1];
		vThrow2[2] = (vThrow2[2] * fVel) + p_local->m_vecVelocity()[2];

		for (float fTime = 0.0f; fTime < TIMEALIVE; fTime += fStep) {

			vPos = vStart + vThrow2 * fStep;

			Ray_t ray;
			trace_t tr;

			CTraceFilter loc;
			loc.pSkip = p_local;

			ray.Init(vStart, vPos);
			g_EngineTrace->TraceRay(ray, MASK_SOLID, &loc, &tr);

			if (tr.DidHit()) {
				vThrow2 = tr.plane.normal * -2.0f * DotProduct(vThrow2, tr.plane.normal) + vThrow2;
				vThrow2 *= GRENADE_COEFFICIENT_OF_RESTITUTION;

				iCollisions++;

				if (iCollisions > 2)
					break;

				vPos = vStart + vThrow2 * tr.fraction * fStep;

				fTime += (fStep * (1 - tr.fraction));
			}

			Vector vOutStart, vOutEnd;

			/* Only draw if we are holding our mouse left button */
			if (g_InputSystem->IsButtonDown(MOUSE_LEFT))
			{
				if (weapon->IsGrenade())
				{
					if (Math::WorldToScreen(vStart, vOutStart), Math::WorldToScreen(vPos, vOutEnd))
					{
						Render::Get().RenderLine(vOutStart.x - 0.5, vOutStart.y - 0.5, vOutEnd.x - 0.5, vOutEnd.y - 0.5, Color(57, 57, 57));
						Render::Get().RenderLine(vOutStart.x, vOutStart.y, vOutEnd.x, vOutEnd.y, g_Config.GetColor("color_esp_grenade_prediction"));
					}

					vStart = vPos;
					vThrow2.z -= fGravity * tr.fraction * fStep;
				}
			}
		}
	}
}

void Visuals::TraceCircle(Vector position, Color color)
{
	Vector prev_scr_pos{ -1, -1, -1 };
	Vector scr_pos;

	float step = M_PI * 2.0 / 150;

	float rad = 150.f;

	Vector origin = position;

	static float hue_offset = 0;
	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z + 2);
		Vector tracepos(origin.x, origin.y, origin.z + 2);

		Ray_t ray;
		trace_t trace;
		CTraceFilter filter;

		ray.Init(tracepos, pos);

		g_EngineTrace->TraceRay(ray, MASK_ALL, &filter, &trace);

		if (Math::WorldToScreen(trace.endpos, scr_pos))
		{
			if (prev_scr_pos != Vector{ -1, -1, -1 })
			{
				int hue = RAD2DEG(rotation) + hue_offset;

				Render::Get().RenderLine(prev_scr_pos.x, prev_scr_pos.y, scr_pos.x, scr_pos.y, color);
			}
			prev_scr_pos = scr_pos;
		}
	}
	hue_offset -= 0.5;
}

void Visuals::LbyIndicator()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
	bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);

	Color clr = Color::Green;

	if (Moving && !InAir)
		clr = Color::Red;

	if (fabs(g_Saver.AARealAngle.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()) < 35.f)
		clr = Color::Red;

	if (g_Saver.InFakewalk)
		clr = Color(255, 150, 0);

	float percent;

	if (Moving || InAir || g_Saver.InFakewalk)
		percent = 1.f;
	else
		percent = (g_Saver.NextLbyUpdate - g_GlobalVars->curtime) / 1.1f;

	percent = 1.f - percent;

	ImVec2 t = g_pDefaultFont->CalcTextSizeA(34.f, FLT_MAX, 0.0f, "LBY");
	float width = t.x * percent;

	Render::Get().RenderLine(9.f, y - 100.f - (CurrentIndicatorHeight - 34.f), 11.f + t.x, y - 100.f - (CurrentIndicatorHeight - 34.f), Color(0, 0, 0, 25), 4.f);

	if (width < t.x && width > 0.f)
		Render::Get().RenderLine(10.f, y - 100.f - (CurrentIndicatorHeight - 34.f), 10.f + width, y - 100.f - (CurrentIndicatorHeight - 34.f), clr, 2.f);

	Render::Get().RenderText("LBY", ImVec2(10, y - 100.f - CurrentIndicatorHeight), 34.f, clr);
	CurrentIndicatorHeight += 34.f;
}

void Visuals::PingIndicator()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	INetChannelInfo* nci = g_EngineClient->GetNetChannelInfo();

	if (!nci)
		return;

	float ping = nci ? (nci->GetAvgLatency(FLOW_INCOMING)) * 1000.f : 0.0f;
	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	//std::string text = "PING: " + std::to_string(ping);
	float percent = ping / 100.f;
	ImVec2 t = g_pDefaultFont->CalcTextSizeA(34.f, FLT_MAX, 0.0f, "PING");
	float width = t.x * percent;

	int green = int(percent * 2.55f);
	int red = 255 - green;

	Render::Get().RenderLine(9.f, y - 100.f - (CurrentIndicatorHeight - 34.f), 11.f + t.x, y - 100.f - (CurrentIndicatorHeight - 34.f), Color(0, 0, 0, 25), 4.f);
	Render::Get().RenderLine(10.f, y - 100.f - (CurrentIndicatorHeight - 34.f), 10.f + width, y - 100.f - (CurrentIndicatorHeight - 34.f), Color(red, green, 0), 2.f);
	Render::Get().RenderText("PING", ImVec2(10, y - 100.f - CurrentIndicatorHeight), 34.f, Color(red, green, 0));
	CurrentIndicatorHeight += 34.f;
}

void Visuals::LCIndicator()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	if ((g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	//ImVec2 t = g_pDefaultFont->CalcTextSizeA(34.f, FLT_MAX, 0.0f, "LBY");
	Render::Get().RenderText("LC", ImVec2(10, y - 100.f - CurrentIndicatorHeight), 34.f, g_Saver.LCbroken ? Color::Green : Color::Red);
	CurrentIndicatorHeight += 34.f;
}

void Visuals::drawpenetration()
{

	C_LocalPlayer pLocal = g_LocalPlayer;

	QAngle ViewAngles;
	g_EngineClient->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->m_aimPunchAngle() * 2.f;

	Vector fowardVec;
	Math::AngleVectors1337(ViewAngles, &fowardVec);
	fowardVec *= 10000;

	Vector start = pLocal->m_vecOrigin() + pLocal->m_vecViewOffset();
	Vector end = start + fowardVec, endScreen;

	int xs;
	int ys;
	g_EngineClient->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	if (Math::WorldToScreen(end, endScreen) && pLocal->IsAlive())
	{
		float damage = 0.f;

		if (Autowall::Get().CanWallbang(damage))
		{
			Render::Get().RenderCircleFilled(xs, ys, 3, 60, Color(0, 255, 0, 255));
			Render::Get().RenderText("dmg: " + std::to_string(damage), xs + 10, ys + 10, 14.f, g_Config.GetColor("color_esp_autowall"));
		}
		else
		{
			Render::Get().RenderCircleFilled(xs, ys, 5, 60, Color(190, 20, 20, 154));
		}
	}
}

void Visuals::AddToDrawList() {
	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity)
			continue;

		if (entity == g_LocalPlayer && !g_Input->m_fCameraInThirdPerson)
			continue;

		if (entity == g_LocalPlayer)
			continue;

		if (i <= g_GlobalVars->maxClients) {
			auto player = Player();
			if (player.Begin((C_BasePlayer*)entity)) {
				if (g_Config.GetBool("esp_snaplines"))	player.RenderSnapline();
				if (g_Config.GetBool("esp_boxes"))		player.RenderBox();
				if (g_Config.GetBool("esp_skeleton"))	player.RenderSkeleton();
				if (g_Config.GetBool("esp_weapons"))	player.RenderWeaponName();
				if(g_Config.GetBool("esp_ammo"))		player.RenderAmmo();
				if (g_Config.GetBool("esp_names"))		player.RenderName();
				if (g_Config.GetBool("esp_health"))		player.RenderHealth();
				if (g_Config.GetBool("esp_armor"))		player.RenderArmour();
				if (g_Config.GetBool("esp_debug"))		player.DrawPlayerDebugInfo();
			}
		}
		else if (g_Config.GetBool("esp_dropped_weapons") && entity->IsWeapon())
			RenderWeapon(static_cast<C_BaseCombatWeapon*>(entity));
		else if (g_Config.GetBool("esp_dropped_weapons") && entity->IsDefuseKit())
			RenderDefuseKit(entity);
		else if (entity->IsPlantedC4() && g_Config.GetBool("esp_planted_c4"))
			RenderPlantedC4(entity);
		else if (entity->IsLoot() && g_Config.GetBool("esp_items"))
			RenderItemEsp(entity);

		if (g_Config.GetBool("esp_grenade"))
		DrawGrenade(entity);
		
	}

	if (g_Config.GetBool("esp_fovcircle"))
		FOVCircle();

	if (g_Config.GetBool("esp_spreadcircle"))
		SpreadCircle();

	if (g_Config.GetBool("esp_hitmarker"))
		Hitmarker();

	if (g_Config.GetBool("esp_crosshair"))
		RenderCrosshair();

	if (g_Config.GetBool("esp_autowall"))
		drawpenetration();
		//AutoWallCrosshair();

	if (g_Config.GetBool("esp_recoil"))
		RecoilCrosshair();

	if (g_Config.GetBool("esp_noscope"))
		NoScopeOverlay();

	if (g_Config.GetBool("rbot")) {
		PingIndicator();
		LCIndicator();
		
		if (g_Config.GetInt("misc_fakelag_mode") == 1)
			LbyIndicator();
	}

	CurrentIndicatorHeight = 0.f;

	GrenadePrediction();
}
