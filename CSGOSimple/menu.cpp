
#include "Menu.hpp"
#include "FontManager.h"
#include "ConfigSystem.h"
#include "options.hpp"
#include "helpers/math.hpp"


ImFont* IconsFont;
Menu::Menu()
{
    Components = MenuHelper::components();
}

void Menu::Initialize()
{
    ImGuiIO& io = ImGui::GetIO();

    FontManager fontmgr;

    IconsFont = fontmgr.GetIconFont(io, 24.f);
    Loaded = true;
}

void Menu::Render()
{

    if (!Loaded || g_Unload)
        return;

    Components.StartWindow("Krilu92 Private", ImVec2(766, 500), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    static char* NavBarItems[] = { "s", "o", "t", "u", "v" };
    static char* NavBarItemsText[] = { "ragebot", "aimbot", "visuals", "misc", "settings" };
    static int NavBarSelected = 0;
    Components.NavbarIcons(NavBarItems, NavBarItemsText, IM_ARRAYSIZE(NavBarItems), NavBarSelected, IconsFont);

    switch ((MenuAvailable)NavBarSelected)
    {
    case MenuAvailable::RAGEBOT:
        RenderRagebot();
        break;

    case MenuAvailable::LEGITBOT:
        RenderLegitbot();
        break;

    case MenuAvailable::VISUALS:
        RenderVisuals();
        break;

    case MenuAvailable::MISC:
        RenderMisc();
        break;

    case MenuAvailable::SETTINGS:
        RenderSettings();
        break;
    }

    Components.EndWindow();
}

void Menu::RenderRagebot()
{
    Components.BeginChild("#ragebot", ImVec2(0, 0));
    Components.Columns(2, false);

    Components.Label("Ragebot");
    Components.Checkbox("Enable", "rbot");
    Components.Checkbox("On Key", "rbot_onkey");

    if (g_Config.GetBool("rbot_onkey"))
        Components.Hotkey("Ragekey", "rbot_key");

    Components.Checkbox("Doubletap", "doubletap_enable");
    if (g_Config.GetBool("doubletap_enable"))
        Components.Hotkey("Doubletap key", "doubletap_key");

    Components.SliderFloat("Hitchance", "rbot_min_hitchance", 0.f, 100.f);
    Components.SliderFloat("Min damage", "rbot_mindamage", 0.f, 100.f);
    Components.SliderInt("Baim after x shots", "rbot_baim_after_shots", 0, 10);
    Components.SliderInt("Force baim after x shots", "rbot_force_baim_after_shots", 0, 10);
    Components.Checkbox("Baim while moving", "rbot_baim_while_moving");
    Components.Checkbox("Auto scope", "rbot_autoscope");
    Components.Checkbox("Auto stop", "rbot_autostop");
    Components.Checkbox("Auto crouch", "rbot_autocrouch");
#ifdef _DEBUG
    Components.Checkbox("Fakelag prediction", "rbot_flag_prediction");
#endif // _DEBUG

    const char* ShootingModes[] = { "normal", "in fakelag", "fakelag while shooting" };
    Components.ComboBox("Shooting mode", ShootingModes, IM_ARRAYSIZE(ShootingModes), "rbot_shooting_mode");

#ifdef _DEBUG
    Components.Checkbox("Lagcompensation", "rbot_lagcompensation");
#endif // _DEBUG
    Components.Checkbox("Force unlag", "rbot_force_unlage");

    Components.Checkbox("Resolver", "rbot_resolver");
    const char* BaimModes[] = { "never", "auto" };
    Components.ComboBox("Baim mode", BaimModes, IM_ARRAYSIZE(BaimModes), "rbot_baimmode");
    Components.Checkbox("Air baim", "rbot_resolver_air_baim");

    Components.NextColumn();

    static const char* YawAAs[] = { "none", "backwards", "spinbot", "lower body yaw", "random", "freestanding", "custom" };
    static const char* YawAddAAs[] = { "none", "jitter", "switch", "spin", "random" };
    static const char* PitchAAs[] = { "none", "emotion", "down", "up", "zero", "jitter", "down jitter", "up jitter", "zero jitter", "spin", "up spin", "down spin", "random", "switch", "down switch", "up switch", "fake up", "fake down", "custom" };
    static char* AntiAimMenus[] = { "stand", "move", "air", "misc" };
    static int AAMenuSelected = 0;
    Components.Navbar(AntiAimMenus, IM_ARRAYSIZE(AntiAimMenus), AAMenuSelected);
    Components.Checkbox("Enable aa", "rbot_aa");
    static const char* FakelagModes[] = { "normal", "adaptive" };

    switch ((RbotMenuAvailable)AAMenuSelected)
    {
    case RbotMenuAvailable::STANDING:
        Components.ComboBox("pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), "rbot_aa_stand_pitch");
        Components.ComboBox("real yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_stand_real_yaw");

        Components.ComboBox("real add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_stand_real_add_yaw_add");
        Components.SliderFloat("range", "rbot_aa_stand_real_add_yaw_add_range", 0.f, 360.f);

        Components.SliderInt("Fakelag ticks", "misc_fakelag_ticks_standing", 0, 14);
        break;

    case RbotMenuAvailable::MOVING:
        Components.ComboBox("pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), "rbot_aa_move_pitch");
        Components.ComboBox("real yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_move_real_yaw");

        Components.ComboBox("real add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_move_real_add_yaw_add");
        Components.SliderFloat("range", "rbot_aa_move_real_add_yaw_add_range", 0.f, 360.f);

        Components.ComboBox("Fakelag mode", FakelagModes, IM_ARRAYSIZE(FakelagModes), "misc_fakelag_mode_moving");
        Components.SliderInt("Fakelag ticks", "misc_fakelag_ticks_moving", 0, 14);
        break;

    case RbotMenuAvailable::AIR:
        Components.ComboBox("pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), "rbot_aa_air_pitch");
        Components.ComboBox("real yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_air_real_yaw");

        Components.ComboBox("real add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_air_real_add_yaw_add");
        Components.SliderFloat("range", "rbot_aa_air_real_add_yaw_add_range", 0.f, 360.f);

        Components.ComboBox("Fakelag mode", FakelagModes, IM_ARRAYSIZE(FakelagModes), "misc_fakelag_mode_air");
        Components.SliderInt("Fakelag ticks", "misc_fakelag_ticks_air", 0, 14);
        break;

    case RbotMenuAvailable::MISC:

        Components.SliderFloat("Spinbot speed", "rbot_aa_spinbot_speed", -20.f, 20.f);
        Components.Checkbox("Slidewalk", "rbot_slidewalk");
        Components.Checkbox("desync", "rbot_aa_desync");
        Components.Hotkey("Manual AA right", "rbot_manual_key_right");
        Components.Hotkey("Manual AA left", "rbot_manual_key_left");
        Components.Hotkey("Manual AA back", "rbot_manual_key_back");
        break;
    }

    Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
    Components.Checkbox("Head", "rbot_hitbox_head");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat(" ", "rbot_hitbox_head_scale", 0.f, 1.f);
    Components.Checkbox("Neck", "rbot_hitbox_neck");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("  ", "rbot_hitbox_neck_scale", 0.f, 1.f);
    Components.Checkbox("Chest", "rbot_hitbox_chest");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("   ", "rbot_hitbox_chest_scale", 0.f, 1.f);
    Components.Checkbox("Pelvis", "rbot_hitbox_pelvis");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("    ", "rbot_hitbox_pelvis_scale", 0.f, 1.f);
    Components.Checkbox("Stomach", "rbot_hitbox_stomach");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("     ", "rbot_hitbox_stomach_scale", 0.f, 1.f);
    Components.Checkbox("Arm", "rbot_hitbox_arm");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("      ", "rbot_hitbox_arm_scale", 0.f, 1.f);
    Components.Checkbox("Leg", "rbot_hitbox_leg");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("       ", "rbot_hitbox_leg_scale", 0.f, 1.f);
    Components.Checkbox("Foot", "rbot_hitbox_foot");
    Components.SameLine();
    Components.Label(" Scale:");
    Components.SameLine();
    Components.SliderFloat("        ", "rbot_hitbox_foot_scale", 0.f, 1.f);
    Components.EndChild();

    Components.EndChild();
}

void Menu::RenderLegitbot()
{
    Components.BeginChild("#aim", ImVec2(0, 0));
    Components.Label("Aimbot");
    Components.Checkbox("Enable", "aim_enable");

    Components.Checkbox("Backtrack", "aim_backtrack");
    Components.Checkbox("Aim at backtrack", "aim_backtrack_aim");
    Components.SliderFloat("Backtrack time", "aim_backtrack_ms", 0.f, .2f);
    Components.Spacing();
    Components.Columns(2, false);


    static char* WeaponConfigSelectionItems[] = { "G", "L", "f", "W", "c", "Z" };
    static char* WeaponConfigSelectionItemsText[] = { "pistol", "smg", "mg", "rifle", "shotgun", "sniper" };
    static int WeaponSelected = 0;
    Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);

    Components.Hotkey("Aimkey", "aim_key");
   
    switch ((aimWeaponsAvailable)WeaponSelected)
    {
    case aimWeaponsAvailable::PISTOL:
        Components.Checkbox("Trigger", "aim_pistol_triggerbot");
        Components.SliderFloat("Fov", "aim_pistol_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_pistol_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_pistol_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_pistol_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_pistol_rcs");
        Components.SliderFloat("Amount x", "aim_pistol_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_pistol_rcs_y", 0.f, 1.f);

        Components.NextColumn();

        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_pistol_hitbox_head");
        Components.Checkbox("neck", "aim_pistol_hitbox_neck");
        Components.Checkbox("chest", "aim_pistol_hitbox_chest");
        Components.Checkbox("pelvis", "aim_pistol_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_pistol_hitbox_stomach");
        Components.Checkbox("arm", "aim_pistol_hitbox_arm");
        Components.Checkbox("leg", "aim_pistol_hitbox_leg");
        Components.Checkbox("foot", "aim_pistol_hitbox_foot");
        Components.EndChild();
        break;

    case aimWeaponsAvailable::SMG:
        Components.Checkbox("Trigger", "aim_smg_triggerbot");
        Components.SliderFloat("Fov", "aim_smg_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_smg_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_smg_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_smg_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_smg_rcs");
        Components.SliderFloat("Amount x", "aim_smg_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_smg_rcs_y", 0.f, 1.f);

        Components.NextColumn();
  
        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_smg_hitbox_head");
        Components.Checkbox("neck", "aim_smg_hitbox_neck");
        Components.Checkbox("chest", "aim_smg_hitbox_chest");
        Components.Checkbox("pelvis", "aim_smg_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_smg_hitbox_stomach");
        Components.Checkbox("arm", "aim_smg_hitbox_arm");
        Components.Checkbox("leg", "aim_smg_hitbox_leg");
        Components.Checkbox("foot", "aim_smg_hitbox_foot");
        Components.EndChild();
        break;

    case aimWeaponsAvailable::MG:
        Components.Checkbox("Trigger", "aim_mg_triggerbot");
        Components.SliderFloat("Fov", "aim_mg_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_mg_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_mg_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_mg_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_mg_rcs");
        Components.SliderFloat("Amount x", "aim_mg_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_mg_rcs_y", 0.f, 1.f);

        Components.NextColumn();

        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_mg_hitbox_head");
        Components.Checkbox("neck", "aim_mg_hitbox_neck");
        Components.Checkbox("chest", "aim_mg_hitbox_chest");
        Components.Checkbox("pelvis", "aim_mg_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_mg_hitbox_stomach");
        Components.Checkbox("arm", "aim_mg_hitbox_arm");
        Components.Checkbox("leg", "aim_mg_hitbox_leg");
        Components.Checkbox("foot", "aim_mg_hitbox_foot");
        Components.EndChild();
        break;

    case aimWeaponsAvailable::RIFLE:
        Components.Checkbox("Trigger", "aim_rifle_triggerbot");
        Components.SliderFloat("Fov", "aim_rifle_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_rifle_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_rifle_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_rifle_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_rifle_rcs");
        Components.SliderFloat("Amount x", "aim_rifle_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_rifle_rcs_y", 0.f, 1.f);

        Components.NextColumn();
      
        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_rifle_hitbox_head");
        Components.Checkbox("neck", "aim_rifle_hitbox_neck");
        Components.Checkbox("chest", "aim_rifle_hitbox_chest");
        Components.Checkbox("pelvis", "aim_rifle_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_rifle_hitbox_stomach");
        Components.Checkbox("arm", "aim_rifle_hitbox_arm");
        Components.Checkbox("leg", "aim_rifle_hitbox_leg");
        Components.Checkbox("foot", "aim_rifle_hitbox_foot");
        Components.EndChild();
        break;

    case aimWeaponsAvailable::SHOTGUN:
        Components.Checkbox("Trigger", "aim_shotgun_triggerbot");
        Components.SliderFloat("Fov", "aim_shotgun_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_shotgun_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_shotgun_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_shotgun_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_shotgun_rcs");
        Components.SliderFloat("Amount x", "aim_shotgun_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_shotgun_rcs_y", 0.f, 1.f);

        Components.NextColumn();
 
        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_shotgun_hitbox_head");
        Components.Checkbox("neck", "aim_shotgun_hitbox_neck");
        Components.Checkbox("chest", "aim_shotgun_hitbox_chest");
        Components.Checkbox("pelvis", "aim_shotgun_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_shotgun_hitbox_stomach");
        Components.Checkbox("arm", "aim_shotgun_hitbox_arm");
        Components.Checkbox("leg", "aim_shotgun_hitbox_leg");
        Components.Checkbox("foot", "aim_shotgun_hitbox_foot");
        Components.EndChild();
        break;

    case aimWeaponsAvailable::SNIPER:
        Components.Checkbox("Trigger", "aim_sniper_triggerbot");
        Components.SliderFloat("Fov", "aim_sniper_fov", 0.f, 15.f);
        Components.SliderFloat("Smooth", "aim_sniper_smooth", 1.f, 30.f);
        Components.SliderFloat("Randomize", "aim_sniper_randomize", 0.f, 10.f);
        Components.SliderFloat("Delay", "aim_sniper_delay", 0.f, 1.f);

        Components.Spacing();

        Components.Checkbox("Rcs", "aim_sniper_rcs");
        Components.SliderFloat("Amount x", "aim_sniper_rcs_x", 0.f, 1.f);
        Components.SliderFloat("Amount y", "aim_sniper_rcs_y", 0.f, 1.f);

        Components.NextColumn();

        Components.Label("Hitboxes:");
        Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
        Components.Checkbox("head", "aim_sniper_hitbox_head");
        Components.Checkbox("neck", "aim_sniper_hitbox_neck");
        Components.Checkbox("chest", "aim_sniper_hitbox_chest");
        Components.Checkbox("pelvis", "aim_sniper_hitbox_pelvis");
        Components.Checkbox("stomach", "aim_sniper_hitbox_stomach");
        Components.Checkbox("arm", "aim_sniper_hitbox_arm");
        Components.Checkbox("leg", "aim_sniper_hitbox_leg");
        Components.Checkbox("foot", "aim_sniper_hitbox_foot");
        Components.EndChild();
        break;
    }

    Components.EndChild();
}

void Menu::RenderVisuals()
{
    Components.BeginChild("#visuals", ImVec2(0, 0));

    Components.Label("Visuals");
    static char* VisualsCategories[] = { "esp", "glow", "chams", "misc" };
    static int SelectedMenu = 0;
    Components.Navbar(VisualsCategories, IM_ARRAYSIZE(VisualsCategories), SelectedMenu);

    static const char* BoxTypes[] = { "normal", "edge", "3d" };

    switch ((VisualsMenuAvailable)SelectedMenu)
    {
    case VisualsMenuAvailable::ESP:
    {

        Components.Columns(2, false);

        Components.Checkbox("Enable  ", "esp_enable");
        Components.Checkbox("Boxes  ", "esp_boxes");
        Components.SameLine();
        Components.ComboBox("Box type", BoxTypes, IM_ARRAYSIZE(BoxTypes), "esp_boxtype");
        Components.Checkbox("Team  ", "esp_enemies_only");
        Components.Checkbox("Names   ", "esp_names");
        Components.Checkbox("Health   ", "esp_health");
        Components.Spacing();
        Components.Spacing();
        Components.Spacing();
        Components.ColorBox("Enemy Visible (Color)", "color_esp_enemy_visible"); 
        Components.ColorBox("Enemy Occluded (Color)", "color_esp_enemy_occluded");
        Components.Spacing();
        if (g_Config.GetBool("esp_enemies_only")) {
            Components.ColorBox("Team Visible (Color)", "color_esp_ally_visible");
            Components.ColorBox("Team Occluded (Color)", "color_esp_ally_occluded");
        }
        Components.Spacing();
        Components.Spacing();
        Components.ColorCheckbox("Skeleton", "esp_skeleton", "color_esp_skeleton");
        Components.ColorCheckbox("Head", "esp_skeleton_head", "color_esp_skeleton_head");
        Components.ColorCheckbox("Weapons", "esp_weapons", "color_esp_weapons");
        Components.ColorCheckbox("Ammo", "esp_ammo", "color_esp_ammo");
        Components.ColorCheckbox("Grenade", "esp_grenade", "color_esp_grenade");
        Components.ColorCheckbox("Grenade Prediction", "esp_grenade_prediction", "color_esp_grenade_prediction");

        Components.NextColumn();

        Components.Checkbox("Armor   ", "esp_armor");
        Components.Checkbox("Snaplines   ", "esp_snaplines");
        Components.Checkbox("Debug   ", "esp_debug");
        Components.Spacing();
        Components.Spacing();
        Components.Spacing();

        Components.ColorCheckbox("Dropped Weapons", "esp_dropped_weapons", "color_esp_dropped_weapons");
        Components.ColorCheckbox("Defuse", "esp_defuse", "color_esp_defuse");
        Components.ColorCheckbox("Planted C4", "esp_c4", "color_esp_c4");
        Components.ColorCheckbox("Items", "esp_items", "color_esp_items");

        break;
    }

    case VisualsMenuAvailable::GLOW:
    {
        Components.Columns(1, false);
        Components.Checkbox("Enable  ", "glow_players");
        Components.Spacing();
        Components.Spacing();
        Components.Checkbox("Team", "glow_enemies_only");
        Components.Spacing();
        Components.ColorBox("Enemy (Color)", "color_glow_enemy");
        if (g_Config.GetBool("glow_enemies_only")) {
            Components.ColorBox("Team (Color)", "color_glow_ally");
        }
        Components.Spacing();
        Components.Spacing();
        Components.ColorCheckbox("Weapons", "glow_weapons", "color_glow_weapons");
        Components.ColorCheckbox("Planted C4", "glow_c4", "color_glow_c4");
        Components.ColorCheckbox("C4 Carrier", "glow_c4_carrier", "color_glow_c4_carrier");
        Components.ColorCheckbox("Defuse kit", "glow_defuse", "color_glow_defuse");
        Components.ColorCheckbox("Chickens", "glow_chicken", "color_glow_chicken");
        break;
    }

    case VisualsMenuAvailable::CHAMS:
    {
        const char* chams[] = { "normal", "flat", "glass", "animated", "platinum", "crystal", "chrome", "silver", "gold", "plastic", "glow" };

        Components.Columns(2, false);
        Components.Label("Player Chams");
        Components.Checkbox("Enable", "chams_player_enabled");
        Components.Checkbox("Team", "chams_player_enemies_only");
        Components.Spacing();
        Components.Spacing();
        Components.Label("Enemy Options");
        Components.Checkbox("Enemy Ignorez   ", "chams_player_ignorez");
        Components.Checkbox("Enemy Wireframe   ", "chams_player_wireframe");
        Components.ColorBox("Enemy Visible (Color)", "color_chams_player_enemy_visible");
        if (g_Config.GetBool("chams_player_ignorez"))
            Components.ColorBox("Enemy Occluded (Color)", "color_chams_player_enemy_occluded");
        Components.ComboBox("Enemy Visible", chams, IM_ARRAYSIZE(chams), "chams_player_style");
        if (g_Config.GetBool("chams_player_ignorez"))
            Components.ComboBox("Enemy Occluded", chams, IM_ARRAYSIZE(chams), "chams_player_ignorez_style"); 
       
        Components.Spacing();
        Components.Spacing();
        if (g_Config.GetBool("chams_player_enemies_only")) {

            Components.Label("Team Options");
            Components.Checkbox("Team Ignorez   ", "chams_player_ignorez_team");
            Components.Checkbox("Team Wireframe   ", "chams_player_wireframe_team");
            Components.ColorBox("Team Visible (Color)", "color_chams_player_ally_visible");
            if (g_Config.GetBool("chams_player_ignorez_team"))
                Components.ColorBox("Team Occluded (Color)", "color_chams_player_ally_occluded");
            Components.ComboBox("Team Visible", chams, IM_ARRAYSIZE(chams), "chams_player_style_team");
            if (g_Config.GetBool("chams_player_ignorez_team"))
                Components.ComboBox("Team Occluded", chams, IM_ARRAYSIZE(chams), "chams_player_ignorez_style_team");
        }

        Components.NextColumn();

        Components.Label("Arm Chams");
        Components.Checkbox("Arms Enable   ", "chams_arms_enabled");
        Components.Spacing();
        Components.Spacing();
        Components.Label("Arm Options");
        Components.Checkbox("Arms Ignorez   ", "chams_arms_ignorez");
        Components.Checkbox("Arms Wireframe   ", "chams_arms_wireframe");
        Components.ColorBox("Arms Visible (Color)", "color_chams_arms_visible");
        if (g_Config.GetBool("chams_arms_ignorez"))
            Components.ColorBox("Arms Occluded (Color)", "color_chams_arms_occluded");
        Components.ComboBox("Arms Visible", chams, IM_ARRAYSIZE(chams), "chams_arms_style");
        if (g_Config.GetBool("chams_arms_ignorez"))
            Components.ComboBox("Arms Occluded", chams, IM_ARRAYSIZE(chams), "chams_arms_ignorez_style");
        break;
    }

    case VisualsMenuAvailable::MISC:
    {
        Components.Columns(3, false);
        Components.Checkbox("Thirdperson", "misc_thirdperson");
        Components.Hotkey("Thirdperson hotkey", "misc_thirdperson_hotkey");
        Components.Spacing();
        Components.Spacing();
        Components.ColorCheckbox("Tracer", "misc_tracer", "color_tracer");

        if (g_Config.GetBool("misc_tracer"))
            Components.SliderFloat("Trace Life", "misc_tracer_livetime", .5f, 10.f);

        Components.NextColumn();
        Components.Checkbox("No blur", "misc_noblur");
        Components.Checkbox("No flash", "misc_noflash");
        Components.Checkbox("No hands", "misc_nohands");
        Components.Checkbox("No smoke", "misc_nosmoke");
        Components.Checkbox("No shadows", "misc_noshadows");
        Components.Checkbox("No Visual Recoil", "misc_novisrecoil");
        Components.Checkbox("No scope overlay", "esp_noscope");
        Components.Checkbox("Disable scope zoom", "misc_nozoom");
        Components.SliderInt("Viewmodel fov", "viewmodel_fov", 1, 150);
        Components.SliderInt("Fov", "fov", 1, 150);

        Components.NextColumn();

        Components.ColorCheckbox("Fov Circle", "esp_fovcircle", "color_esp_fovcircle");
        Components.ColorCheckbox("Spread Circle", "esp_spreadcircle", "color_esp_spreadcircle");
        Components.ColorCheckbox("Hitmarker", "esp_hitmarker", "color_esp_hitmarker");
        Components.ColorCheckbox("Autowall", "esp_autowall", "color_esp_autowall");
        Components.ColorCheckbox("Crosshair", "esp_crosshair", "color_esp_crosshair");
        Components.ColorCheckbox("Recoil Crosshair", "esp_recoil", "color_esp_recoil_cross");

        break;
    }
    }


    Components.EndChild();
}

void Menu::RenderMisc()
{
    Components.BeginChild("#misc", ImVec2(0, 0));
    Components.Columns(2, false);
    
    Components.Label("Misc");
    Components.ColorCheckbox("Watermark", "misc_watermark", "color_watermark");
    Components.Spacing();
    Components.Checkbox("Bhop", "misc_bhop");
    Components.Checkbox("Autostrafer", "misc_autostrafe");

    Components.Checkbox("No Sky", "misc_nosky");
    Components.Checkbox("Props Alpha", "misc_world_alpha");
    Components.ColorCheckbox("Props (Color)", "misc_color_props", "color_props");
    Components.ColorCheckbox("World (Color)", "misc_world", "color_world");
    Components.ColorCheckbox("Sky (Color)", "misc_nosky_color", "color_sky");
   
    Components.Spacing();

    Components.Checkbox("Rank reveal", "misc_showranks");
    Components.Checkbox("No crouch cooldown", "misc_crouchcooldown");

    Components.Spacing();

    Components.Label("Postprocessing:");
    Components.SliderFloat("Red", "mat_ambient_light_r", 0.f, 1.f);
    Components.SliderFloat("Green", "mat_ambient_light_g", 0.f, 1.f);
    Components.SliderFloat("Blue", "mat_ambient_light_b", 0.f, 1.f);

    Components.NextColumn();

    static const char* dot[] = { "circle", "square" };
    Components.Checkbox("Engine Radar", "misc_engine_radar");
    Components.Spacing();
    Components.Spacing();
    Components.Checkbox("2D Radar", "misc_radar");
    Components.Checkbox("Enemies Only", "misc_radar_enemies_only");
    Components.Checkbox("Last Position", "misc_radar_show_lastpos");
    Components.Spacing();
    Components.Spacing();
    Components.ComboBox("Dot", dot, IM_ARRAYSIZE(dot), "misc_radar_dot");
    Components.SliderInt("Dot Size", "misc_radar_dot_size", 0, 10);
    Components.Spacing();
    Components.Spacing();
    Components.ColorBox("Localplayer (Color)", "color_localplayer");
    Components.ColorBox("FOV Lines (Color)", "color_fovlines");
    Components.ColorBox("Last Position (Color)", "color_lastpos");
    Components.ColorBox("Enemy Visible (Color)", "color_enemyVis");
    Components.ColorBox("Enemy Occluded (Color)", "color_enemy");
    Components.ColorBox("Team (Color)", "color_teammate");

    if (Components.Button("unload"))
        g_Unload = true;

    Components.EndChild();
}

void Menu::RenderSettings()
{
    Components.BeginChild("#settings", ImVec2(0, 0));

    Components.Label("Settings");
    Components.Spacing();

    Components.Columns(3, false);

    Components.BeginChild("#allConfigs", ImVec2(0.f, 250.f));

    //for() Configs
    static int Selected = -1;
    int i = 0;

    for (auto config = g_Config.Configs.begin(); config != g_Config.Configs.end(); config++, i++)
    {
        if (ImGui::Selectable(config->data(), i == Selected))
            Selected = i;
    }

    Components.EndChild();

    Components.NextColumn();

    static char str0[128] = "";
    Components.Label("Name ");
    Components.SameLine();
    ImGui::InputText("  ", str0, IM_ARRAYSIZE(str0));

    if (Components.Button("Create") && str0 != "")
        g_Config.CreateConfig(str0);

    if (Components.Button("Save") && Selected != -1)
        g_Config.Save(g_Config.Configs[Selected]);

    Components.SameLine();

    if (Components.Button("Load") && Selected != -1)
        g_Config.Load(g_Config.Configs[Selected]);

    if (Components.Button("Refresh"))
        g_Config.RefreshConfigList();

    Components.SameLine();

    if (Components.Button("Reset"))
        g_Config.ResetConfig();

    Components.EndChild();
}

#define MAKE_IMU32(r, g, b, a) ((a << 24) + (b << 16) + (g << 8) + r)

void rotateVector(float& x, float& y, float angle, float originX = 0, float originY = 0)
{
    x -= originX;
    y -= originY;
    float tx = x, ty = y;
    float fCos = cos(angle);
    float fSin = sin(angle);

    tx = x * fCos - y * fSin;
    ty = x * fSin + y * fCos;

    x = tx + originX;
    y = ty + originY;
}

void Menu::DrawRadar() {

    if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_Config.GetBool("misc_radar"))
        return;

    //Open new window
    Components.StartWindow("Radar", ImVec2(300, 300), ImGuiWindowFlags_NoCollapse);

    // Background
    ImVec2 bgPos = ImGui::GetCursorScreenPos();
    ImVec2 bgSize = ImGui::GetContentRegionAvail();

    ImGui::GetWindowDrawList()->AddRectFilled(bgPos, ImVec2(bgPos.x + bgSize.x, bgPos.y + bgSize.y), 0xFF333333);

    //Localplayer
    auto local = g_LocalPlayer;

    // Radar
    Vector pos;
    float x, y;
    ImVec2 midRadar = ImVec2(bgPos.x + (bgSize.x / 2), bgPos.y + (bgSize.y / 2));
    float mapSizeX = -5000;
    float mapSizeY = 5000;
    ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x - bgSize.x / 2.f, midRadar.y), ImVec2(midRadar.x + bgSize.x / 2.f, midRadar.y), MAKE_IMU32(0, 0, 0, 64));
    ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - bgSize.y / 2.f), ImVec2(midRadar.x, midRadar.y + bgSize.y / 2.f), MAKE_IMU32(0, 0, 0, 64));
    // @todo: Use the farthest for the map size

    // Localplayer
    pos = g_LocalPlayer->m_vecOrigin();
    x = midRadar.x;
    y = midRadar.y;

    int r, g, b;

    ImU32 colorLocal = MAKE_IMU32(0, 0, 255, 255);
    r = (int)(g_Config.GetColor("color_localplayer")[0]);
    g = (int)(g_Config.GetColor("color_localplayer")[1]);
    b = (int)(g_Config.GetColor("color_localplayer")[2]);
    colorLocal = MAKE_IMU32(r, g, b, 255);


    switch (g_Config.GetInt("misc_radar_dot")) {
    case 0:
        ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(x, y), g_Config.GetInt("misc_radar_dot_size"), colorLocal, 35);
        break;
    case 1:
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x - g_Config.GetInt("misc_radar_dot_size"), y - g_Config.GetInt("misc_radar_dot_size")), ImVec2(x + g_Config.GetInt("misc_radar_dot_size"), y + g_Config.GetInt("misc_radar_dot_size")), colorLocal);
        break;
    }

    // Show local camera
    bool radarViewFixed = true;
    ImVec2 camPos;
    Vector forward;
    QAngle ViewAngles;
    g_EngineClient->GetViewAngles(ViewAngles);

    Math::angleToForwardVector(ViewAngles, forward);
    forward = forward * 1000;

    if (radarViewFixed) {
        camPos = ImVec2(midRadar.x, 0);
    }
    else {
        camPos = ImVec2(x - forward.x, y + forward.y);
    }

    //FOV LINES
    
    ImU32 colorFOV = MAKE_IMU32(0, 0, 255, 255);
    r = (int)(g_Config.GetColor("color_fovlines")[0]);
    g = (int)(g_Config.GetColor("color_fovlines")[1]);
    b = (int)(g_Config.GetColor("color_fovlines")[2]);
    colorFOV = MAKE_IMU32(r, g, b, 255);

    float fov = g_Config.GetInt("fov");
    rotateVector(camPos.x, camPos.y, fov / 2.f, x, y);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(x, y), camPos, colorFOV);
    rotateVector(camPos.x, camPos.y, -fov, x, y);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(x, y), camPos, colorFOV);

    // Players positions
    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        C_BasePlayer* entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

        if (!entity)
            return;

        bool IsVisible = g_LocalPlayer->CanSeePlayer(entity, HITBOX_CHEST);

        if (entity->m_iHealth() <= 0) {
            continue;
        }
        if (entity == local) {
            continue;
        }

        if (!entity->IsPlayer()) {
            continue;
        }

        if (!g_Config.GetBool("misc_radar_enemies_only") || entity->IsEnemy()) {
            if (entity->IsDormant() && !g_Config.GetBool("misc_radar_show_lastpos")) {
                continue;
            }

            ImU32 color = MAKE_IMU32(0, 0, 255, 255);
            // Color
            if (entity->IsEnemy()) {
                if (entity->IsDormant()) {
                    r = (int)(g_Config.GetColor("color_lastpos")[0]);
                    g = (int)(g_Config.GetColor("color_lastpos")[1]);
                    b = (int)(g_Config.GetColor("color_lastpos")[2]);
                    color = MAKE_IMU32(r, g, b, 255);
                }
                else if (IsVisible) {
                    r = (int)(g_Config.GetColor("color_enemyVis")[0]);
                    g = (int)(g_Config.GetColor("color_enemyVis")[1]);
                    b = (int)(g_Config.GetColor("color_enemyVis")[2]);
                    color = MAKE_IMU32(r, g, b, 255);
                }
                else {
                    r = (int)(g_Config.GetColor("color_enemy")[0]);
                    g = (int)(g_Config.GetColor("color_enemy")[1]);
                    b = (int)(g_Config.GetColor("color_enemy")[2]);
                    color = MAKE_IMU32(r, g, b, 255);
                }
            }
            else {
                r = (int)(g_Config.GetColor("color_teammate")[0]);
                g = (int)(g_Config.GetColor("color_teammate")[1]);
                b = (int)(g_Config.GetColor("color_teammate")[2]);
                color = MAKE_IMU32(r, g, b, 255);
            }

            // Show
            pos = entity->m_vecOrigin();
            pos = pos - local->m_vecOrigin();

            x = midRadar.x + (pos.x / mapSizeX) * bgSize.x;
            y = midRadar.y + (pos.y / mapSizeY) * bgSize.y;

            if (radarViewFixed) {
                rotateVector(x, y, DEG2RAD((ViewAngles.yaw + 90)), midRadar.x, midRadar.y);
            }

            switch (g_Config.GetInt("misc_radar_dot")) {
            case 0:
                ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(x, y), g_Config.GetInt("misc_radar_dot_size"), color, 35);
                break;
            case 1:
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x - g_Config.GetInt("misc_radar_dot_size"), y - g_Config.GetInt("misc_radar_dot_size")), ImVec2(x + g_Config.GetInt("misc_radar_dot_size"), y + g_Config.GetInt("misc_radar_dot_size")), color);
                break;
            }
        }
    }
    Components.EndWindow();
}