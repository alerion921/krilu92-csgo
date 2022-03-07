#pragma once

#include <algorithm>
#include "../render.hpp"
#include "../helpers/math.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../RuntimeSaver.hpp"

class Visuals : public Singleton<Visuals>
{
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();

public:
	class Player
	{
	public:
		struct
		{
			C_BasePlayer* pl;
			bool          is_enemy;
			bool          is_visible;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
		} ctx;

		bool Begin(C_BasePlayer * pl);
		void RenderBox();
		void RenderName();
		void RenderWeaponName();
		void RenderAmmo();
		void RenderHealth();
		void RenderArmour();
		void RenderSnapline();
		void DrawPlayerDebugInfo();
		void RenderSkeleton();
	};
	void RenderCrosshair();
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderDefuseKit(C_BaseEntity* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderItemEsp(C_BaseEntity* ent);
	void DrawGrenade(C_BaseEntity* ent);
	void FOVCircle();
	void SpreadCircle();
	void ThirdPerson();
	void Hitmarker();
	void AutoWallCrosshair();
	void NoScopeOverlay();
	void RecoilCrosshair();
	void drawpenetration();

	void LbyIndicator();
	void PingIndicator();
	void LCIndicator();
	void GrenadePrediction();
	void TraceCircle(Vector position, Color color);

	float CurrentIndicatorHeight = 0.f;

public:
	void AddToDrawList();
	void Render();
};


FORCEINLINE vec_t DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
