#include "valve_sdk/sdk.hpp"

#include "Config.hpp"

#include "options.hpp"

#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ShlObj_core.h>

void CConfig::SetupValue(int& value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue<int>(category, name, &value));
}

void CConfig::SetupValue(bool& value, bool def, std::string category, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue<bool>(category, name, &value));
}

void CConfig::SetupValue(float& value, float def, std::string category, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue<float>(category, name, &value));
}

void CConfig::SetupColor(float value[4], std::string name) {
	for (int i = 0; i < 4; i++)
		SetupValue(value[i], value[i], "Colors", name + "_" + std::to_string(i).c_str());
}

void CConfig::SetupRage()
{
	SetupValue(Cfg.RageEnabled, false, "Rage", "enabled");
	SetupValue(Cfg.AutoScope, false, "Rage", "scope");
	SetupValue(Cfg.autocrouch, false, "Rage", "crouch");
	SetupValue(Cfg.rage_backtrack, false, "Rage", "backtrack");
	SetupValue(Cfg.rage_backtrack_time, 0, "Rage", "backtrack_time");
	SetupValue(Cfg.baim_onair, false, "Rage", "baim_air");
	SetupValue(Cfg.baim_whenslowwalk, false, "Rage", "baim_slow");
	SetupValue(Cfg.baim_onkey, false, "Rage", "baim_key");
	SetupValue(Cfg.baim_onkey_key, 0, "Rage", "baim_bind");
	SetupValue(Cfg.autostop, false, "Rage", "autostop");
	SetupValue(Cfg.autorevolver, false, "Rage", "autorevolver");

	SetupValue(Cfg.hitscan_enable, false, "Rage", "hitscan");
	SetupValue(Cfg.rage_hitscan1, false, "Rage", "head");
	SetupValue(Cfg.rage_hitscan2, false, "Rage", "neck");
	SetupValue(Cfg.rage_hitscan3, false, "Rage", "chest");
	SetupValue(Cfg.rage_hitscan4, false, "Rage", "stomach");
	SetupValue(Cfg.rage_hitscan5, false, "Rage", "pelvis");
	SetupValue(Cfg.rage_hitscan6, false, "Rage", "arms");
	SetupValue(Cfg.rage_hitscan7, false, "Rage", "legs");
	SetupValue(Cfg.Hitchance, 15, "Rage", "hitchange");
	SetupValue(Cfg.Damage, 25, "Rage", "damage");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.AAEnabled, false, "Rage", "antiaim");

	SetupValue(Cfg.pitch_standing, 0, "Rage", "pitch_stand");
	SetupValue(Cfg.pitch_moving, 0, "Rage", "pitch_moving");
	SetupValue(Cfg.pitch_air, 0, "Rage", "pitch_air");

	SetupValue(Cfg.aapitch_cust_stand, 0, "Rage", "pitch_cust_stand");
	SetupValue(Cfg.aapitch_cust_mov, 0, "Rage", "pitch_cust_moving");
	SetupValue(Cfg.aapitch_cust_air, 0, "Rage", "pitch_cust_air");

	SetupValue(Cfg.yaw_standing, 0, "Rage", "yaw_stand");
	SetupValue(Cfg.yaw_moving, 0, "Rage", "yaw_moving");
	SetupValue(Cfg.yaw_air, 0, "Rage", "yaw_air");

	SetupValue(Cfg.yawadd_stand, 0, "Rage", "yaw_add_stand");
	SetupValue(Cfg.yawadd_mov, 0, "Rage", "yaw_add_mov");
	SetupValue(Cfg.yawadd_air, 0, "Rage", "yaw_add_air");

	SetupValue(Cfg.desync_jitter, false, "Rage", "desync_jitter");
	SetupValue(Cfg.desync_with_max_delta, false, "Rage", "max_desync_delta");
	SetupValue(Cfg.desync_delta, 0, "Rage", "desync_delta");

	SetupValue(Cfg.jitter_radius_stand, 0, "Rage", "jitter_rad_stand");
	SetupValue(Cfg.jitter_radius_mov, 0, "Rage", "jitter_rad_moving");
	SetupValue(Cfg.jitter_radius_air, 0, "Rage", "jitter_rad_air");

	SetupValue(Cfg.AALeftSideBind_stand, 0, "Rage", "leftside_stand");
	SetupValue(Cfg.AABackSideBind_stand, 0, "Rage", "backside_stand");
	SetupValue(Cfg.AARightSideBind_stand, 0, "Rage", "rightside_stand");

	SetupValue(Cfg.AALeftSideBind_mov, 0, "Rage", "leftside_moving");
	SetupValue(Cfg.AABackSideBind_mov, 0, "Rage", "backside_moving");
	SetupValue(Cfg.AARightSideBind_mov, 0, "Rage", "rightside_moving");

	SetupValue(Cfg.AALeftSideBind_air, 0, "Rage", "leftside_air");
	SetupValue(Cfg.AABackSideBind_air, 0, "Rage", "backside_air");
	SetupValue(Cfg.AARightSideBind_air, 0, "Rage", "rightside_air");

	SetupValue(Cfg.d_enable, 0, "Rage", "desync");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.PistolHitchance, 0, "Rage", "hitchance_pistol");
	SetupValue(Cfg.PistolDamage, 0, "Rage", "damage_pistol");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.HeavyPistHitchance, 0, "Rage", "hitchance_hpistol");
	SetupValue(Cfg.HeavyPistDamage, 0, "Rage", "damage_hpistol");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.RifleHitchance, 0, "Rage", "hitchance_rifle");
	SetupValue(Cfg.RifleDamage, 0, "Rage", "damage_rifle");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.ScoutHitchance, 0, "Rage", "hitchance_pscout");
	SetupValue(Cfg.ScoutDamage, 0, "Rage", "damage_scout");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.AutoHitchance, 0, "Rage", "hitchance_auto");
	SetupValue(Cfg.AutoDamage, 0, "Rage", "damage_auto");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.AwpHitchance, 0, "Rage", "hitchance_awp");
	SetupValue(Cfg.AwpDamage, 0, "Rage", "damage_awp");
}

void CConfig::SetupLegit()
{
	SetupValue(Cfg.enable_legitbot, false, "Legit", "enabled");
	SetupValue(Cfg.aimonkey, false, "Legit", "aim_on_key");
	SetupValue(Cfg.aimkey, 0, "Legit", "aim_key");
	SetupValue(Cfg.randomize_smooth, 0, "Legit", "randomize_smooth");
	SetupValue(Cfg.randomize_angle, 0, "Legit", "randomize_angle");
	SetupValue(Cfg.ForceBody, false, "Legit", "forcebody");
	//-------------------------------------------------------------------
	SetupValue(Cfg.legit_fov_pistols, 1, "Legit", "legit_fov_pistols");
	SetupValue(Cfg.legit_smooth_pistols, 1, "Legit", "legit_smooth_pistols");
	SetupValue(Cfg.legit_rcsmax_pistols, 1, "Legit", "legit_rcsmax_pistols");
	SetupValue(Cfg.hitbox_pistols, 1, "Legit", "hitbox_pistols");
	//-------------------------------------------------------------------
	SetupValue(Cfg.legit_fov_rifles, 1, "Legit", "legit_fov_rifles");
	SetupValue(Cfg.legit_smooth_rifles, 1, "Legit", "legit_smooth_rifles");
	SetupValue(Cfg.legit_rcsmax_rifles, 1, "Legit", "legit_rcsmax_rifles");
	SetupValue(Cfg.hitbox_rifles, 1, "Legit", "hitbox_rifles");
	//-------------------------------------------------------------------
	SetupValue(Cfg.legit_fov_scout, 1, "Legit", "legit_fov_scout");
	SetupValue(Cfg.legit_smooth_scout, 1, "Legit", "legit_smooth_scout");
	SetupValue(Cfg.legit_rcsmax_scout, 1, "Legit", "legit_rcsmax_scout");
	SetupValue(Cfg.hitbox_scout, 1, "Legit", "hitbox_scout");
	//-------------------------------------------------------------------
	SetupValue(Cfg.legit_fov_auto, 1, "Legit", "legit_fov_auto");
	SetupValue(Cfg.legit_smooth_auto, 1, "Legit", "legit_smooth_auto");
	SetupValue(Cfg.legit_rcsmax_auto, 1, "Legit", "legit_rcsmax_auto");
	SetupValue(Cfg.hitbox_auto, 1, "Legit", "hitbox_auto");
	//-------------------------------------------------------------------
	SetupValue(Cfg.legit_fov_awp, 1, "Legit", "legit_fov_awp");
	SetupValue(Cfg.legit_smooth_awp, 1, "Legit", "legit_smooth_awp");
	SetupValue(Cfg.legit_rcsmax_awp, 1, "Legit", "legit_rcsmax_awp");
	SetupValue(Cfg.hitbox_awp, 1, "Legit", "hitbox_awp");
}

void CConfig::SetupVisuals()
{
	SetupValue(Cfg.Box, false, "Visuals", "box");
	SetupValue(Cfg.box_style, 0, "Visuals", "box_style");
	SetupValue(Cfg.Name, false, "Visuals", "name");
	SetupValue(Cfg.Weapon, false, "Visuals", "weapon");
	SetupValue(Cfg.Health, false, "Visuals", "health");
	SetupValue(Cfg.Recoil, false, "Visuals", "recoil");
	SetupValue(Cfg.Size, 0, "Visuals", "recoil_size");
	SetupValue(Cfg.Radar, false, "Visuals", "radar");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.nosmoke, false, "Visuals", "nosmoke");
	SetupValue(Cfg.noflash, false, "Visuals", "noflash");
	SetupValue(Cfg.novisualrecoil, false, "Visuals", "novisrecoil");
	SetupValue(Cfg.RemoveScope, false, "Visuals", "noscope");
	SetupValue(Cfg.remove_fog, false, "Visuals", "nofog");
	SetupValue(Cfg.fullbright, false, "Visuals", "fullbright");
	SetupValue(Cfg.hitmarker, false, "Visuals", "hitmarker");
	SetupValue(Cfg.anglelines, false, "Visuals", "aaline");
	SetupValue(Cfg.showranks, false, "Visuals", "showranks");
	SetupValue(Cfg.foottrils, false, "Visuals", "foottrils");
	SetupValue(Cfg.foottrails_xshift, 0.1, "Visuals", "xshift");
	SetupValue(Cfg.foottrails_yshift, 0.1, "Visuals", "yshift");
	SetupValue(Cfg.foottrails_zshift, 14, "Visuals", "zshift");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.viewmodel_fov, 68, "Visuals", "viewmodel");
	SetupValue(Cfg.ambient_light, false, "Visuals", "amblight");
	SetupValue(Cfg.mat_ambient_light_r, 0, "Visuals", "amblight_r");
	SetupValue(Cfg.mat_ambient_light_g, 0, "Visuals", "amblight_g");
	SetupValue(Cfg.mat_ambient_light_b, 0, "Visuals", "amblight_b");
	SetupValue(Cfg.customfov, false, "Visuals", "custfov");
	SetupValue(Cfg.xfov, 1, "Visuals", "custfov_x");
	SetupValue(Cfg.yfov, 2, "Visuals", "custfov_y");
	SetupValue(Cfg.zfov, -2, "Visuals", "custfov_z");
	SetupValue(Cfg.aspect_ratio, false, "Visuals", "aspectratio");
	SetupValue(Cfg.aspect_ratio_value, 0, "Visuals", "aspectratio_val");
	SetupValue(Cfg.thirdperson, false, "Visuals", "thirdperson");
	SetupValue(Cfg.thirdperson_dist, 0, "Visuals", "thirdperson_dist");
	SetupValue(Cfg.thirdperson_bind, 0, "Visuals", "thirdperson_bnd");
	SetupValue(Cfg.misc_model, false, "Visuals", "agent_changer");
	SetupValue(Cfg.misc_model_type_tt, 0, "Visuals", "tt");
	SetupValue(Cfg.misc_model_type_ct, 0, "Visuals", "ct");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.chams_enable, false, "Visuals", "chams");
	SetupValue(Cfg.chams_ignorez, false, "Visuals", "ignorez");
	SetupValue(Cfg.wireframe_mode, false, "Visuals", "Wireframe");
	SetupValue(Cfg.chams_material, 0, "Visuals", "chams_mat");
	SetupValue(Cfg.chams_alpha, 1.0f, "Visuals", "alpha");
	//-----------------------------------------------------------------------------------
	SetupValue(Cfg.chams_backtrack, 0, "Visuals", "backtrack");
	SetupValue(Cfg.backtrack_material, 0, "Visuals", "backtrack_mat");
	SetupValue(Cfg.backtrack_alpha, 1.0f, "Visuals", "backtrack_alpha");
	//-----------------------------------------------------------------------------------

}

void CConfig::SetupMisc()
{
	SetupValue(Cfg.AutoBunnyhop, false, "Misc", "bhop");
	SetupValue(Cfg.Autostrafe, false, "Misc", "strafe");
	SetupValue(Cfg.retrack_speed, 2, "Misc", "re_track");
	SetupValue(Cfg.InfiniteDuck, false, "Misc", "inf_duck");
	SetupValue(Cfg.FakeDuck, false, "Misc", "fake_duck");
	SetupValue(Cfg.FakeDuck_Bind, 0, "Misc", "fake_duck_bnd");
	SetupValue(Cfg.SlowWalk, false, "Misc", "slowwalk");
	SetupValue(Cfg.SlowWalk_Bind, 0, "Misc", "slowwalk_bind");
	SetupValue(Cfg.slowwalk_speed, 0, "Misc", "slowwalk_speed");
	SetupValue(Cfg.FakelagChoke, 0, "Misc", "fake_lag");
	SetupValue(Cfg.clantag, false, "Misc", "clantag");
	SetupValue(Cfg.chatspam, false, "Misc", "chatspam");
	SetupValue(Cfg.watermark, true, "Misc", "watermark");
	SetupValue(Cfg.legit_desync, false, "Misc", "legit_desync");
	SetupValue(Cfg.damage_show, true, "Misc", "damage log");
	SetupValue(Cfg.hitsound, true, "Misc", "hitsound");
	SetupValue(Cfg.hiteffect, true, "Misc", "hiteffect");
	SetupValue(Cfg.kill_counter, true, "Misc", "kill_counter");
	SetupValue(Cfg.misc_nightmode, false, "Misc", "nightmode");
	SetupValue(Cfg.misc_nightmode_value, 0.03f, "Misc", "nightmode_val");

	SetupValue(Cfg.buybot, false, "Misc", "buybot");
	SetupValue(Cfg.buybot_pistol, 0, "Misc", "buybot_pistol");
	SetupValue(Cfg.buybot_weapon, 0, "Misc", "buybot_weapon");

	SetupValue(Cfg.noshadows, false, "Misc", "Noshadows");
	SetupValue(Cfg.disablepanoramablur, false, "Misc", "Disable_panorama_blur");
	SetupValue(Cfg.no3dsky, false, "Misc", "No 3dsky");
	SetupValue(Cfg.removeBlur, false, "Misc", "No blur");
	SetupValue(Cfg.bloom, false, "Misc", "Bloom");
	SetupValue(Cfg.bloom_value, 0, "Misc", "bloom_value");
	SetupValue(Cfg.model_ambient_value, 0, "Misc", "model_ambient");
	SetupValue(Cfg.r_farz, false, "Misc", "r_farz");
	SetupValue(Cfg.r_farz_value, 0.03f, "Misc", "r_farz_value");

}

void CConfig::SetupColors()
{
	SetupColor(Cfg.chams_color, "chams_color");
	SetupColor(Cfg.chams_color_ignorez, "ignorez_color");
	SetupColor(Cfg.backtrack_color, "backtrack_color");
}

void CConfig::Initialize()
{
	CConfig::SetupRage();
	CConfig::SetupLegit();
	CConfig::SetupVisuals();
	CConfig::SetupMisc();
	CConfig::SetupColors();
}

void CConfig::Save(std::string szIniFile)
{
	std::string folder, file;
	folder = XorStr("C:/killaura/");

	file = folder + szIniFile + XorStr(".host");

	CreateDirectoryA(folder.c_str(), NULL);

	for (auto value : ints) {
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());
	}

	for (auto value : floats) WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());
	for (auto value : bools) WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());
}

void CConfig::Load(std::string szIniFile)
{
	std::string folder, file;

	folder = XorStr("C:/killaura/");

	file = folder + szIniFile + XorStr(".host");

	CreateDirectoryA(folder.c_str(), NULL);

	char value_l[32] = { '\0' };
	for (auto value : ints) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "0", value_l, 32, file.c_str()); *value->value = atoi(value_l);
	}

	for (auto value : floats) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "0.0f", value_l, 32, file.c_str()); *value->value = atof(value_l);
	}

	for (auto value : bools) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "false", value_l, 32, file.c_str()); *value->value = !strcmp(value_l, "true");
	}
}
