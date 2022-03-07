
#include "singleton.hpp"
#include "menu_helpers.hpp"

enum class MenuAvailable : int
{
    RAGEBOT,
    LEGITBOT,
    VISUALS,
    MISC,
    SETTINGS
};

enum class VisualsMenuAvailable : int
{
    ESP,
    GLOW,
    CHAMS,
    MISC
};

enum class RbotMenuAvailable : int
{
    STANDING,
    MOVING,
    AIR,
    MISC
};

enum class aimWeaponsAvailable : int
{
    PISTOL,
    SMG,
    MG,
    RIFLE,
    SHOTGUN,
    SNIPER
};

#pragma once
class Menu : public Singleton<Menu>
{
public:
    Menu();
    void Initialize();
    void Render();

    void DrawRadar();

    /* sections */
    //void RenderLegitbot();
    void RenderRagebot();
    void RenderLegitbot();
    void RenderVisuals();
    void RenderMisc();
    void RenderSettings();
private:
    MenuHelper::components Components;
    //MenuFonts* fonts;
    //MenuFonts* Mfonts;
    bool Loaded = false;
};

