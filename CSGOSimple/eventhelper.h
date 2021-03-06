#pragma once


#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "options.hpp"
#include "Resolver.h"
#include "HitPossitionHelper.h"
#include "RuntimeSaver.hpp"
#include "ConfigSystem.h"
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

#pragma once
class EventHelper : public Singleton<EventHelper>
{
    class listener : public IGameEventListener2
    {
    public:
        void Start();
        void Stop();
        void FireGameEvent(IGameEvent* event) override;
        int GetEventDebugID(void) override;
    };
public:
    void init()
    {
        Listener.Start();
    }
    void unInit()
    {
        Listener.Stop();
    }
    void on_fire_event(IGameEvent* event)
    {
        Resolver::Get().OnFireEvent(event);
        HitPossitionHelper::Get().OnFireEvent(event);

        if (!strcmp(event->GetName(), "player_hurt"))
        {

            if (!g_Config.GetBool("esp_hitmarker"))
                return;

            int attacker = event->GetInt("attacker");

            if (g_EngineClient->GetPlayerForUserID(attacker) == g_EngineClient->GetLocalPlayer())
            {
                g_Saver.HitmarkerInfo = HitmarkerInfoStruct{ g_GlobalVars->realtime, 0.f };

                g_VGuiSurface->PlaySound_("buttons\\arena_switch_press_02.wav");
            }
        }

        if (!strcmp(event->GetName(), "bullet_impact"))
        {
            C_BasePlayer* shooter = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));

            if (!shooter || shooter != g_LocalPlayer)
                return;

            Vector p = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
            ShotTracer(g_Saver.LastShotEyePos, p);
        }

    }
    void ShotTracer(Vector shot_pos, Vector hit_pos);
private:
    listener Listener;
};