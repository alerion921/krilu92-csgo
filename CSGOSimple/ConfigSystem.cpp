
#include "ConfigSystem.h"
#include "valve_sdk/csgostructs.hpp"
#include <atlstr.h>
#include "includes/INIReader.h"
#include <iostream>
#include <filesystem>
#include <string>
#include "imgui/imgui.h"

namespace fs = std::filesystem;

int ConfigSystem::GetInt ( std::string name )
{
    return intOptions[name];
}

float ConfigSystem::GetFloat ( std::string name )
{
    return floatOptions[name];
}

bool ConfigSystem::GetBool ( std::string name )
{
    return boolOptions[name];
}

Color ConfigSystem::GetColor ( std::string name )
{
    return colorOptions[name];
}

void ConfigSystem::Set ( std::string name, int value )
{
    intOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, float value )
{
    floatOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, bool value )
{
    boolOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, Color value )
{
    colorOptions[name] = value;
}

void ConfigSystem::Save ( std::string file )
{
    std::string orgfile = file;
    file = AppdataFolder + "configs\\" + file;
    std::wstring filename ( file.begin(), file.end() );

    for ( auto option : intOptions )
    {
        std::string v = std::to_string ( option.second );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"int", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : floatOptions )
    {
        std::string v = std::to_string ( option.second );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"float", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : boolOptions )
    {
        std::string v = option.second ? "true" : "false";
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"bool", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : colorOptions )
    {
        Color c = option.second;
        std::string v = std::string ( std::to_string ( c.r() ) + ";" + std::to_string ( c.g() ) + ";" + std::to_string ( c.b() ) + ";" + std::to_string ( c.a() ) );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"color", name.c_str(), value.c_str(), filename.c_str() );
    }
}

void ConfigSystem::Load ( std::string file )
{
    std::string orgfile = file;
    file = AppdataFolder + "configs\\" + file;
    INIReader reader ( file );

    if ( reader.ParseError() < 0 )
    {
        return;
    }

    for ( auto option : intOptions )
    {
        std::string n = option.first;
        intOptions[n] = reader.GetInteger ( "int", n, 0 );
    }

    for ( auto option : floatOptions )
    {
        std::string n = option.first;
        floatOptions[n] = ( float ) reader.GetReal ( "float", n, 0 );
    }

    for ( auto option : boolOptions )
    {
        std::string n = option.first;
        boolOptions[n] = reader.Get ( "bool", n, "false" ) == "true";
    }

    for ( auto option : colorOptions )
    {
        std::string n = option.first;
        //Console.WriteLine(n);
        std::string val = reader.Get ( "color", n, "255;255;255;255" );
        std::istringstream f ( val );
        std::string s;
        int i = 0;
        int r, g, b, a;

        while ( getline ( f, s, ';' ) )
        {
            switch ( i )
            {
                case 0:
                    r = std::stoi ( s );
                    break;

                case 1:
                    g = std::stoi ( s );
                    break;

                case 2:
                    b = std::stoi ( s );
                    break;

                case 3:
                    a = std::stoi ( s );
                    break;
            }

            i++;
        }

        colorOptions[n] = Color ( r, g, b, a );
    }
}

void ConfigSystem::RefreshConfigList()
{
    Configs.clear();

    for ( auto& p : fs::directory_iterator ( AppdataFolder + "configs\\" ) )
    {
        fs::path path = fs::path ( p );
        Configs.push_back ( path.filename().string() );
    }
}

void ConfigSystem::CreateConfig ( std::string file )
{
    std::string orgfile = file;

    if ( fs::exists ( AppdataFolder + "configs\\" + file ) )
        return;

    std::ofstream ofs ( AppdataFolder + "configs\\" + file );
    ofs << "";
    ofs.close();

    RefreshConfigList();
}

void ConfigSystem::Setup()
{
   
    AppdataFolder = std::string ( getenv ( "APPDATA" ) ) + "\\Krilu92-Private\\";

    if ( !fs::exists ( AppdataFolder ) )
    {
        fs::create_directory ( AppdataFolder );
    }

    if ( !fs::exists ( AppdataFolder + "configs\\" ) )
        fs::create_directory ( AppdataFolder + "configs\\" );

    RefreshConfigList();

    
    //
    // ESP
    SetupVar("esp_enable", false);
    SetupVar ("esp_enemies_only", false);
    SetupVar("esp_dropped_weapons", false);
    SetupVar("esp_snaplines", false);
    SetupVar("esp_boxes", false);
    SetupVar("esp_boxtype", 0);
    SetupVar("esp_skeleton", false);
    SetupVar("esp_skeleton_head", false);
    SetupVar("esp_weapons", false);
    SetupVar("esp_names", false);
    SetupVar("esp_health", false);
    SetupVar("esp_armor", false);
    SetupVar("esp_debug", false);
    SetupVar("esp_grenade_prediction", false);
    SetupVar("esp_planted_c4", false);
    SetupVar("esp_items", false);
    SetupVar("esp_grenade", false);
    SetupVar("esp_fovcircle", false);
    SetupVar("esp_spreadcircle", false);
    SetupVar("esp_hitmarker", false);
    SetupVar("esp_crosshair", false);
    SetupVar("esp_autowall", false);
    SetupVar("esp_recoil", false);
    SetupVar("esp_noscope", false);


    SetupVar("color_esp_enemy_visible", Color(255, 0, 0,255));
    SetupVar("color_esp_enemy_occluded", Color(0, 255, 0, 255));
    SetupVar("color_esp_ally_visible", Color(0, 0, 255, 255));
    SetupVar("color_esp_ally_occluded", Color(100, 0, 255, 255));
    SetupVar("color_esp_skeleton", Color(255, 255, 255, 255));
    SetupVar("color_esp_skeleton_head", Color(255, 255, 255, 255));
    SetupVar("color_esp_crosshair", Color(50, 255, 0, 255));
    SetupVar("color_esp_weapons", Color(255, 100, 0, 255));
    SetupVar("color_esp_dropped_weapons", Color(255, 100, 0, 255));
    SetupVar("color_esp_ammo", Color(255, 100, 0, 255));
    SetupVar("color_esp_defuse", Color(0, 255, 255, 255));
    SetupVar("color_esp_c4", Color(0, 255, 0, 255));
    SetupVar("color_esp_items", Color(0, 255, 0, 255));
    SetupVar("color_esp_fovcircle", Color(255, 255, 0, 255));
    SetupVar("color_esp_spreadcircle", Color(255, 255, 0, 255));
    SetupVar("color_esp_hitmarker", Color(255, 255, 0, 255));
    SetupVar("color_esp_autowall", Color(255, 255, 0, 255));
    SetupVar("color_esp_recoil_cross", Color(255, 255, 0, 255));
    SetupVar("color_esp_grenade", Color(255, 255, 0, 255));
    SetupVar("color_esp_grenade_prediction", Color(255, 255, 0, 255));
    //

    

    //
    // MISC
    SetupVar("misc_engine_radar", false);
    SetupVar("misc_watermark", true);
    SetupVar ("misc_thirdperson", false );
    SetupVar ("misc_thirdperson_hotkey", 0x43 );
    SetupVar("misc_thirdperson_dist", 50.f);
    SetupVar ("misc_noflash", false );
    SetupVar("misc_noshadows", false);
    SetupVar("misc_novisrecoil", false);
    SetupVar("misc_noblur", false);
    SetupVar("misc_nozoom", false);
    SetupVar("misc_nosmoke", false);
    SetupVar("misc_nohands", false);

    SetupVar("misc_nosky", false);
    SetupVar("misc_world", false);
    SetupVar("misc_color_props", false);
    SetupVar("misc_world_alpha", false);
    SetupVar("misc_nosky_color", false);
    SetupVar("color_world", Color(50, 50, 255));
    SetupVar("color_sky", Color(50, 50, 255));
    SetupVar("color_props", Color(50, 50, 255));

    SetupVar("misc_crouchcooldown", false);
    SetupVar("misc_bhop", false);
    SetupVar("misc_autostrafe", false);
    SetupVar("misc_showranks", false);
    SetupVar("viewmodel_fov", 68);
    SetupVar("fov", 90);
    SetupVar("fov_scoped", 46);
    SetupVar("misc_brightness", 1.f);
    SetupVar("mat_ambient_light_r", 0.f);
    SetupVar("mat_ambient_light_g", 0.f);
    SetupVar("mat_ambient_light_b", 0.f);
    SetupVar("misc_tracer", false);
    SetupVar("misc_tracer_livetime", 0.f);

    SetupVar("color_tracer", Color(50, 50, 255, 255));
    SetupVar("color_watermark", Color(50, 50, 255, 255));
    //

    //
    // CHAMS
    SetupVar("chams_player_enabled", false);
    SetupVar("chams_player_enemies_only", false);

    SetupVar("chams_player_ignorez", false);
    SetupVar("chams_player_wireframe", false);
    SetupVar("chams_player_ignorez_team", false);
    SetupVar("chams_player_wireframe_team", false);
    SetupVar("chams_player_ignorez_style", 0);
    SetupVar("chams_player_style", 0);
    SetupVar("chams_player_ignorez_style_team", 0);
    SetupVar("chams_player_style_team", 0);

    SetupVar("chams_arms_enabled", false);

    SetupVar("chams_arms_ignorez", false);
    SetupVar("chams_arms_wireframe", false);
    SetupVar("chams_arms_ignorez_style", 0);
    SetupVar("chams_arms_style", 0);

    SetupVar("color_chams_player_enemy_visible", Color(200, 0, 0, 255));
    SetupVar("color_chams_player_enemy_occluded", Color(50, 50, 50, 255));
    SetupVar("color_chams_player_ally_visible", Color(0, 0, 200, 255));
    SetupVar("color_chams_player_ally_occluded", Color(100, 200, 0, 255));

    SetupVar("color_chams_arms_occluded", Color(0, 0, 200, 255));
    SetupVar("color_chams_arms_visible", Color(100, 200, 0, 255));
    //

    //
    // GLOW
    SetupVar("glow_players", false); 
    SetupVar("glow_enemies_only", false);
    SetupVar("glow_c4_carrier", false);
    SetupVar("glow_chicken", false);
    SetupVar("glow_defuse", false);
    SetupVar("glow_c4", false);
    SetupVar("glow_weapons", false);

    SetupVar("color_glow_c4_carrier", Color(128, 255, 128, 255));
    SetupVar("color_glow_chicken", Color(255, 255, 100, 255));
    SetupVar("color_glow_defuse", Color(255, 50, 255, 255));
    SetupVar("color_glow_c4", Color(0, 255, 0, 255));
    SetupVar("color_glow_weapons", Color(255, 100, 0, 255));
    SetupVar("color_glow_enemy", Color(255, 0, 0, 255));
    SetupVar("color_glow_ally", Color(0, 0, 255, 255));
    //

    //
    // RADAR
    SetupVar("misc_radar", false);
    SetupVar("misc_radar_enemies_only", false);
    SetupVar("misc_radar_show_lastpos", false);
    SetupVar("misc_radar_dot", 0);
    SetupVar("misc_radar_dot_size", 4);

    SetupVar("color_localplayer", Color(0, 255, 0, 255));
    SetupVar("color_fovlines", Color(0, 0, 255, 255));
    SetupVar("color_lastpos", Color(128, 128, 128, 255));
    SetupVar("color_enemyVis", Color(255, 100, 0, 255));
    SetupVar("color_enemy", Color(255, 0, 0, 255));
    SetupVar("color_teammate", Color(0, 0, 255, 255));
    //

    //
    // FAKELAG
    SetupVar ( "misc_fakelag_ticks_standing", 0 );
    SetupVar ( "misc_fakelag_mode_moving", 0 );
    SetupVar ( "misc_fakelag_ticks_moving", 0 );
    SetupVar ( "misc_fakelag_mode_air", 0 );
    SetupVar ( "misc_fakelag_ticks_air", 0 );
    //

    //
    // RBOT
    //
    SetupVar ( "rbot", false );
    SetupVar( "rbot_onkey", false);
    SetupVar( "rbot_key", 0x0);
    SetupVar("doubletap_enable", false);
    SetupVar("doubletap_key", 0x0);
    SetupVar ( "rbot_mindamage", 0.f );
    SetupVar ( "rbot_min_hitchance", 0.f );
    SetupVar ( "rbot_baim_after_shots", 2 );
    SetupVar ( "rbot_force_baim_after_shots", 0 );
    SetupVar ( "rbot_baim_while_moving", false );
    SetupVar ( "rbot_force_unlage", false );
    SetupVar ( "rbot_lby_prediction", false );
    SetupVar ( "rbot_flag_prediction", false );
    SetupVar ( "rbot_autoscope", true );
    SetupVar ( "rbot_baimmode", 0 );

    // resolver
    SetupVar ( "rbot_resolver", false );
    SetupVar ( "rbot_resolver_air_baim", false );
    SetupVar ( "rbot_shooting_mode", 0 );
    SetupVar ( "rbot_lagcompensation", false );
    // antiaims

    SetupVar ( "rbot_aa_desync", false );
    SetupVar ( "rbot_aa", false );
    SetupVar ( "rbot_slidewalk", false );
    SetupVar ( "rbot_manual_aa_state", 0 );
    SetupVar ( "rbot_manual_key_right", 0x0 );
    SetupVar ( "rbot_manual_key_left", 0x0 );
    SetupVar ( "rbot_manual_key_back", 0x0 );
    SetupVar ( "rbot_aa_lby_breaker", false ); //
    SetupVar ( "rbot_aa_fake_lby_breaker", false );
    SetupVar ( "rbot_aa_lby_breaker_yaw", 0.f ); //
    SetupVar ( "rbot_aa_lby_breaker_freestanding", false );
    SetupVar ( "rbot_autostop", false );
    SetupVar ( "rbot_autocrouch", false );

    /*
    	- Fake break
    	- pre break
    */

    SetupVar ( "rbot_aa_spinbot_speed", 5.f );
    SetupVar ( "rbot_aa_stand_pitch", 0 );
    SetupVar ( "rbot_aa_move_pitch", 0 );
    SetupVar ( "rbot_aa_air_pitch", 0 );
    SetupVar ( "rbot_aa_stand_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_move_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_air_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_stand_real_yaw", 0 );
    SetupVar ( "rbot_aa_move_real_yaw", 0 );
    SetupVar ( "rbot_aa_air_real_yaw", 0 );
    SetupVar ( "rbot_aa_stand_fake_yaw", 0 );
    SetupVar ( "rbot_aa_move_fake_yaw", 0 );
    SetupVar ( "rbot_aa_air_fake_yaw", 0 );
    SetupVar ( "rbot_aa_stand_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_move_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_air_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_stand_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_move_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_air_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_stand_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_move_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_air_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_stand_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_move_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_air_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_stand_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_move_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_air_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_stand_fake_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_move_fake_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_air_fake_add_yaw_add_range", 0.f );

    SetupVar ( "rbot_hitbox_head_scale", 0.f );
    SetupVar ( "rbot_hitbox_neck_scale", 0.f );
    SetupVar ( "rbot_hitbox_chest_scale", 0.f );
    SetupVar ( "rbot_hitbox_pelvis_scale", 0.f );
    SetupVar ( "rbot_hitbox_stomach_scale", 0.f );
    SetupVar ( "rbot_hitbox_arm_scale", 0.f );
    SetupVar ( "rbot_hitbox_leg_scale", 0.f );
    SetupVar ( "rbot_hitbox_foot_scale", 0.f );

    SetupVar ( "rbot_aa_fakewalk_key", 0x0 );

    // hitboxes
    SetupVar ( "rbot_hitbox_head", false );
    SetupVar ( "rbot_hitbox_neck", false );
    SetupVar ( "rbot_hitbox_chest", false );
    SetupVar ( "rbot_hitbox_pelvis", false );
    SetupVar ( "rbot_hitbox_stomach", false );
    SetupVar ( "rbot_hitbox_arm", false );
    SetupVar ( "rbot_hitbox_leg", false );
    SetupVar ( "rbot_hitbox_foot", false );

    //aim
    SetupVar ( "aim_enable", false );
    SetupVar ( "aim_legitaa", false );
    SetupVar ( "aim_legitaa_mode", 0 );
    SetupVar ( "aim_legitaa_yaw", 0.f );
    SetupVar ( "aim_key", 0x0 );
    SetupVar ( "aim_backtrack", false );
    SetupVar ( "aim_backtrack_aim", false );
    SetupVar ( "aim_backtrack_ms", 0.2f );

    SetupVar ( "aim_pistol_hitbox_head", false );
    SetupVar ( "aim_pistol_hitbox_neck", false );
    SetupVar ( "aim_pistol_hitbox_chest", false );
    SetupVar ( "aim_pistol_hitbox_pelvis", false );
    SetupVar ( "aim_pistol_hitbox_stomach", false );
    SetupVar ( "aim_pistol_hitbox_arm", false );
    SetupVar ( "aim_pistol_hitbox_leg", false );
    SetupVar ( "aim_pistol_hitbox_foot", false );
    SetupVar ( "aim_pistol_fov", 0.f );
    SetupVar ( "aim_pistol_smooth", 1.f );
    SetupVar ( "aim_pistol_randomize", 0.f );
    SetupVar ( "aim_pistol_rcs", false );
    SetupVar ( "aim_pistol_rcs_x", 0.f );
    SetupVar ( "aim_pistol_rcs_y", 0.f );
    SetupVar ( "aim_pistol_delay", 0.f );
    SetupVar ( "aim_pistol_triggerbot", false );

    SetupVar ( "aim_smg_hitbox_head", false );
    SetupVar ( "aim_smg_hitbox_neck", false );
    SetupVar ( "aim_smg_hitbox_chest", false );
    SetupVar ( "aim_smg_hitbox_pelvis", false );
    SetupVar ( "aim_smg_hitbox_stomach", false );
    SetupVar ( "aim_smg_hitbox_arm", false );
    SetupVar ( "aim_smg_hitbox_leg", false );
    SetupVar ( "aim_smg_hitbox_foot", false );
    SetupVar ( "aim_smg_fov", 0.f );
    SetupVar ( "aim_smg_smooth", 1.f );
    SetupVar ( "aim_smg_randomize", 0.f );
    SetupVar ( "aim_smg_rcs", false );
    SetupVar ( "aim_smg_rcs_x", 0.f );
    SetupVar ( "aim_smg_rcs_y", 0.f );
    SetupVar ( "aim_smg_delay", 0.f );
    SetupVar("aim_smg_triggerbot", false);

    SetupVar ( "aim_mg_hitbox_head", false );
    SetupVar ( "aim_mg_hitbox_neck", false );
    SetupVar ( "aim_mg_hitbox_chest", false );
    SetupVar ( "aim_mg_hitbox_pelvis", false );
    SetupVar ( "aim_mg_hitbox_stomach", false );
    SetupVar ( "aim_mg_hitbox_arm", false );
    SetupVar ( "aim_mg_hitbox_leg", false );
    SetupVar ( "aim_mg_hitbox_foot", false );
    SetupVar ( "aim_mg_fov", 0.f );
    SetupVar ( "aim_mg_smooth", 1.f );
    SetupVar ( "aim_mg_randomize", 0.f );
    SetupVar ( "aim_mg_rcs", false );
    SetupVar ( "aim_mg_rcs_x", 0.f );
    SetupVar ( "aim_mg_rcs_y", 0.f );
    SetupVar ( "aim_mg_delay", 0.f );
    SetupVar("aim_mg_triggerbot", false);

    SetupVar ( "aim_rifle_hitbox_head", false );
    SetupVar ( "aim_rifle_hitbox_neck", false );
    SetupVar ( "aim_rifle_hitbox_chest", false );
    SetupVar ( "aim_rifle_hitbox_pelvis", false );
    SetupVar ( "aim_rifle_hitbox_stomach", false );
    SetupVar ( "aim_rifle_hitbox_arm", false );
    SetupVar ( "aim_rifle_hitbox_leg", false );
    SetupVar ( "aim_rifle_hitbox_foot", false );
    SetupVar ( "aim_rifle_fov", 0.f );
    SetupVar ( "aim_rifle_smooth", 1.f );
    SetupVar ( "aim_rifle_randomize", 0.f );
    SetupVar ( "aim_rifle_rcs", false );
    SetupVar ( "aim_rifle_rcs_x", 0.f );
    SetupVar ( "aim_rifle_rcs_y", 0.f );
    SetupVar ( "aim_rifle_delay", 0.f );
    SetupVar("aim_rifle_triggerbot", false);

    SetupVar ( "aim_shotgun_hitbox_head", false );
    SetupVar ( "aim_shotgun_hitbox_neck", false );
    SetupVar ( "aim_shotgun_hitbox_chest", false );
    SetupVar ( "aim_shotgun_hitbox_pelvis", false );
    SetupVar ( "aim_shotgun_hitbox_stomach", false );
    SetupVar ( "aim_shotgun_hitbox_arm", false );
    SetupVar ( "aim_shotgun_hitbox_leg", false );
    SetupVar ( "aim_shotgun_hitbox_foot", false );
    SetupVar ( "aim_shotgun_fov", 0.f );
    SetupVar ( "aim_shotgun_smooth", 1.f );
    SetupVar ( "aim_shotgun_randomize", 0.f );
    SetupVar ( "aim_shotgun_rcs", false );
    SetupVar ( "aim_shotgun_rcs_x", 0.f );
    SetupVar ( "aim_shotgun_rcs_y", 0.f );
    SetupVar ( "aim_shotgun_delay", 0.f );
    SetupVar("aim_shotgun_triggerbot", false);

    SetupVar ( "aim_sniper_hitbox_head", false );
    SetupVar ( "aim_sniper_hitbox_neck", false );
    SetupVar ( "aim_sniper_hitbox_chest", false );
    SetupVar ( "aim_sniper_hitbox_pelvis", false );
    SetupVar ( "aim_sniper_hitbox_stomach", false );
    SetupVar ( "aim_sniper_hitbox_arm", false );
    SetupVar ( "aim_sniper_hitbox_leg", false );
    SetupVar ( "aim_sniper_hitbox_foot", false );
    SetupVar ( "aim_sniper_fov", 0.f );
    SetupVar ( "aim_sniper_smooth", 1.f );
    SetupVar ( "aim_sniper_randomize", 0.f );
    SetupVar ( "aim_sniper_rcs", false );
    SetupVar ( "aim_sniper_rcs_x", 0.f );
    SetupVar ( "aim_sniper_rcs_y", 0.f );
    SetupVar ( "aim_sniper_delay", 0.f );
    SetupVar("aim_sniper_triggerbot", false);
}

void ConfigSystem::SetupVar ( std::string name, int value )
{
    intOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, float value )
{
    floatOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, bool value )
{
    boolOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, Color value )
{
    colorOptions[name] = value;
}
ConfigSystem g_Config;