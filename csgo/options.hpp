#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"
#include <map>

#define CONFIG(type, var, val) type var = val

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T* () { return &*value; }
	operator T() const { return *value; }
};
class Options
{
public:
#pragma region Aimbot
	CONFIG(bool, RageEnabled, false);
	CONFIG(bool, resolver, false);
	//doubletap
	CONFIG(bool, doubletap, false);
	CONFIG(int, doubletap_shot, 0);
	CONFIG(int, doubletap_delay, 0);
	//Anti-aim
	CONFIG(bool, AAEnabled, false);
	CONFIG(int, rage_desync_type, 0);

	CONFIG(int, AALeftSideBind_stand, 0);
	CONFIG(int, AARightSideBind_stand, 0);
	CONFIG(int, AABackSideBind_stand, 0);

	CONFIG(int, AALeftSideBind_mov, 0);
	CONFIG(int, AARightSideBind_mov, 0);
	CONFIG(int, AABackSideBind_mov, 0);

	CONFIG(int, AALeftSideBind_air, 0);
	CONFIG(int, AARightSideBind_air, 0);
	CONFIG(int, AABackSideBind_air, 0);

	CONFIG(int, aapitch, 0);
	CONFIG(int, aapitch_cust_air, 0);
	CONFIG(int, aapitch_cust_stand, 0);
	CONFIG(int, aapitch_cust_mov, 0);

	int yaw_standing, yaw_moving, yaw_air, antiaim_freestanding_mode;
	int pitch_standing, pitch_moving, pitch_air;

	CONFIG(int, aayaw, 0);
	CONFIG(bool, d_enable, false);
	bool desync_jitter;

	CONFIG(int, jitter_radius_stand, 0);
	CONFIG(int, jitter_radius_mov, 0);
	CONFIG(int, jitter_radius_air, 0);

	CONFIG(int, yawadd_stand, 0);
	CONFIG(int, yawadd_mov, 0);
	CONFIG(int, yawadd_air, 0);

	CONFIG(int, desync_delta, 0);
	CONFIG(bool, desync_with_max_delta, false);

	CONFIG(bool, AutoScope, false);
	CONFIG(bool, autocrouch, false);

	CONFIG(bool, autostop, false);
	CONFIG(int, autostop_mode, 0);

	CONFIG(bool, autorevolver, false);
	CONFIG(bool, SlowWalk, false);
	CONFIG(int, slowwalk_speed, 0);
	CONFIG(int, SlowWalk_Bind, 0);

	CONFIG(bool, hitscan_enable, false);
	CONFIG(bool, rage_hitscan1, true);
	CONFIG(bool, rage_hitscan2, false);
	CONFIG(bool, rage_hitscan3, false);
	CONFIG(bool, rage_hitscan4, false);
	CONFIG(bool, rage_hitscan5, false);
	CONFIG(bool, rage_hitscan6, false);
	CONFIG(bool, rage_hitscan7, false);

	CONFIG(bool, baim_onair, false);
	CONFIG(bool, baim_whenslowwalk, false);
	CONFIG(int, baim_onkey_key, 0);
	CONFIG(bool, baim_onkey, false);

	CONFIG(bool, rage_backtrack, false);
	CONFIG(int, rage_backtrack_time, 200);

	CONFIG(bool, InfiniteDuck, false);
	CONFIG(bool, FakeDuck, false);
	CONFIG(int, FakeDuck_Bind, 0);

	CONFIG(int, Hitchance, 25);
	CONFIG(int, Damage, 15);

	CONFIG(int, AwpHitchance, 25);
	CONFIG(int, AwpDamage, 15);

	CONFIG(int, AutoHitchance, 25);
	CONFIG(int, AutoDamage, 15);

	CONFIG(int, ScoutHitchance, 25);
	CONFIG(int, ScoutDamage, 15);

	CONFIG(int, RifleHitchance, 25);
	CONFIG(int, RifleDamage, 15);

	CONFIG(int, PistolHitchance, 25);
	CONFIG(int, PistolDamage, 15);

	CONFIG(int, HeavyPistHitchance, 25);
	CONFIG(int, HeavyPistDamage, 15);

#pragma endregion

#pragma region Legit
	CONFIG(bool, enable_legitbot, false);
	CONFIG(bool, aimonkey, false);
	CONFIG(int, aimkey, 0);

	CONFIG(int, randomize_smooth, 0);
	CONFIG(int, randomize_angle, 0);

	bool ForceBody = false;
	CONFIG(int, forcebody_legitbot_key, 0);
	//pistols
	CONFIG(int, hitbox_pistols, 1);
	CONFIG(bool, aim_LegitBotPistols, true);
	CONFIG(float, legit_fov_pistols, 2.f);
	CONFIG(float, legit_smooth_pistols, 1.7f);
	CONFIG(float, legit_rcsmin_pistols, 0.f);
	CONFIG(float, legit_rcsmax_pistols, 0.f);

	CONFIG(int, hitbox_rifles, 1);
	CONFIG(bool, aim_LegitBotRifles, true);
	CONFIG(float, legit_smooth_rifles, 1.7f);
	CONFIG(float, legit_fov_rifles, 2.f);
	CONFIG(float, legit_rcsmin_rifles, 0.f);
	CONFIG(float, legit_rcsmax_rifles, 0.f);

	//scout
	CONFIG(int, hitbox_scout, 1);
	CONFIG(bool, aim_LegitBotscout, true);
	CONFIG(float, legit_smooth_scout, 1.5);
	CONFIG(float, legit_fov_scout, 15.f);
	CONFIG(float, legit_rcsmin_scout, 0.f);
	CONFIG(float, legit_rcsmax_scout, 0.f);

	//auto
	CONFIG(int, hitbox_auto, 1);
	CONFIG(bool, aim_LegitBotauto, true);
	CONFIG(float, legit_smooth_auto, 1.5);
	CONFIG(float, legit_fov_auto, 15.f);
	CONFIG(float, legit_rcsmin_auto, 0.f);
	CONFIG(float, legit_rcsmax_auto, 0.f);

	//awp
	CONFIG(int, hitbox_awp, 1);
	CONFIG(bool, aim_LegitBotawp, true);
	CONFIG(float, legit_smooth_awp, 1.5);
	CONFIG(float, legit_fov_awp, 15.f);
	CONFIG(float, legit_rcsmin_awp, 0.f);
	CONFIG(float, legit_rcsmax_awp, 0.f);

	bool legit_hitbox_head_pistol, legit_hitbox_head_rifle, legit_hitbox_head_scout, legit_hitbox_head_auto, legit_hitbox_head_awp;

#pragma endregion 

#pragma region Visuals
	CONFIG(bool, Health, false);
	CONFIG(bool, Name, false);
	CONFIG(bool, Box, false);
	int box_style = 1;
	CONFIG(bool, Rainbow, false);
	CONFIG(bool, Weapon, false);
	CONFIG(bool, skeleton, false);
	CONFIG(bool, Radar, false);
	CONFIG(bool, Recoil, false);
	CONFIG(int, Size, 5);
	CONFIG(bool, showranks, false);
	bool nosmoke;
	
	CONFIG(bool, noflash, false);
	CONFIG(bool, novisualrecoil, false);
	CONFIG(bool, ambient_light, false);
	CONFIG(float, mat_ambient_light_r, 0.0f);
	CONFIG(float, mat_ambient_light_g, 0.0f);
	CONFIG(float, mat_ambient_light_b, 0.0f);
	CONFIG(int, override_fov, 90);
	CONFIG(int, viewmodel_fov, 68);
	CONFIG(bool, thirdperson, false);
	CONFIG(int, thirdperson_dist, 50);
	CONFIG(int, thirdperson_bind, 0);
	CONFIG(bool, customfov, false);
	CONFIG(int, xfov, 1);
	CONFIG(int, yfov, 2);
	CONFIG(int, zfov, -2);
	CONFIG(bool, aspect_ratio, false);
	CONFIG(int, aspect_ratio_value, 0);
	CONFIG(bool, watermark, true);
	CONFIG(bool, hitmarker, false);
	CONFIG(bool, anglelines, false);
	CONFIG(bool, remove_fog, false);
	CONFIG(bool, fullbright, false);
	CONFIG(bool, bullettracers, false);
	CONFIG(Color, bullettracers_color, Color(255, 255, 255));
	float color_menu[4]{ 87 / 255.f, 48 / 255.f , 161 / 255.f, 1.0f };
//--------chams--------
	CONFIG(bool, chams_enable, false);
	CONFIG(bool, wireframe_mode, false);
	float chams_color[3] = { 1.f, 1.f, 1.f };
	CONFIG(int, chams_material, 0);
	CONFIG(bool, chams_ignorez, false);
	float chams_color_ignorez[3] = { 1.f, 1.f, 1.f };
	CONFIG(float, chams_alpha, 1.0f);
//------backtrack------
	CONFIG(int, chams_backtrack, 0);
	CONFIG(int, backtrack_material, 0);
	float backtrack_color[3] = { 1.f, 1.f, 1.f };
	CONFIG(float, backtrack_alpha, 1.0f);
//---------arms--------
	CONFIG(bool, chams_arms_enable, false);
	CONFIG(bool, arms_wireframe_mode, false);
	float chams_arms_color[3] = { 1.f, 1.f, 1.f };
	CONFIG(int, chams_arms_material, 0);
	CONFIG(float, chams_arms_alpha, 1.0f);
/*//------glow---------
	bool VisualsGlowEnabled = false;
	int VisualsGlowGlowstyle = 0;
	float VisualsGlowColor[3] = { 1.f, 1.f, 1.f };
	int VisualsGlowAlpha = 255;*/
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBunnyhop, false);
	CONFIG(int, Autostrafe, 0);
	CONFIG(float, retrack_speed, 2.0f);
	
	CONFIG(bool, model, false);
	CONFIG(int, model_type_tt, 0);
	CONFIG(int, model_type_ct, 0);
	/*CONFIG(bool, circle_enable, false);
	CONFIG(int, circle_key, 0);*/

	CONFIG(bool, clantag, false);
	CONFIG(bool, chatspam, false);
	CONFIG(bool, damage_show, false);
	CONFIG(bool, hitsound, false);
	CONFIG(bool, hiteffect, false);
	CONFIG(bool, RemoveScope, false);

	CONFIG(bool, misc_nightmode, false);
	CONFIG(float, misc_nightmode_value, 0.0f);

	CONFIG(bool, bloom, false);
	CONFIG(int, bloom_value, 0);
	CONFIG(int, model_ambient_value, 0);

	CONFIG(bool, r_farz, false);
	CONFIG(float, r_farz_value, 0.f);

	CONFIG(int, FakelagChoke, 0);
	bool disablepanoramablur, noshadows, no3dsky, removeBlur, noprocessing;
	CONFIG(bool, legit_desync, false);
	CONFIG(int, legit_aa_type, 0);

	CONFIG(bool, buybot, false);
	CONFIG(int, buybot_pistol, 0);
	CONFIG(int, buybot_weapon, 0);

	CONFIG(bool, foottrils, false);
	CONFIG(float, foottrails_xshift, 0.1f);
	CONFIG(float, foottrails_yshift, 0.1f);
	CONFIG(float, foottrails_zshift, 14.f);

	bool kill_counter;

	bool misc_model;
	int misc_model_type;
	int misc_model_type_ct;
	int misc_model_type_tt;
	CONFIG(int, skybox, 0);
	bool buybot_grenade[8] = { false, false, false, false, false, false, false, false };

#pragma endregion

#pragma region Cfg
	CONFIG(int, ConfigFile, 0);
#pragma endregion
};

extern Options Cfg;
extern std::string ActiveWeaponName;
extern bool   uninject;

namespace Globals
{
	extern bool WeaponTabValid;
}