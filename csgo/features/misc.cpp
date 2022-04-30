#include "misc.hpp"
#include "../options.hpp"
#include "../valve_sdk/structs.hpp"
#include "../helpers/math.hpp"
#ifndef max
#define max(a ,b) (((a) > (b)) ? (a) : (b))
#endif

void Misc::OnRoundStart()
{
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !Cfg.buybot)
	{
		return;
	}

	LastRoundStartTime = g_GlobalVars->curtime;
	ShouldBuy = true;
}

void Misc::OnCreateMove()
{
	if (!g_LocalPlayer || !ShouldBuy)
		return;

	ShouldBuy = false;

	std::string buycommand = "";
	BuyWeapons weapon = (BuyWeapons)Cfg.buybot_weapon;

	if (Cfg.buybot_grenade[0])
	{
		buycommand += "buy molotov; ";
	}
	if (Cfg.buybot_grenade[1])
	{
		buycommand += "buy hegrenade; ";
	}
	if (Cfg.buybot_grenade[2])
	{
		buycommand += "buy smokegrenade; ";
	}
	if (Cfg.buybot_grenade[3])
	{
		buycommand += "buy flashbang; ";
	}
	if (Cfg.buybot_grenade[4])
	{
		buycommand += "buy decoy; ";
	}
	if (Cfg.buybot_grenade[5])
	{
		buycommand += "buy vest; buy vesthelm; ";
	}
	if (Cfg.buybot_grenade[6])
	{
		buycommand += "buy taser; ";
	}
	if (Cfg.buybot_grenade[7])
	{
		buycommand += "buy defuser; ";
	}

	switch (weapon)
	{
	case BuyWeapons::SG_AUG:
		buycommand += "buy sg556; buy aug; ";
		break;
	case BuyWeapons::SSG:
		buycommand += "buy ssg08; ";
		break;
	case BuyWeapons::AUTO:
		buycommand += "buy scar20; buy g3sg1; ";
		break;
	case BuyWeapons::MAC_10:
		buycommand += "buy mac10; ";
		break;
	case BuyWeapons::P90:
		buycommand += "buy p90; ";
		break;
	case BuyWeapons::PP_BIZON:
		buycommand += "buy bizon; ";
		break;
	case BuyWeapons::AK_47:
		buycommand += "buy ak47; buy m4a4; buy m4a1_silencer; ";
		break;
	case BuyWeapons::AWP:
		buycommand += "buy awp; ";
		break;
	}


	BuyPistols pistol = (BuyPistols)Cfg.buybot_pistol;
	switch (pistol)
	{
	case BuyPistols::GLOCK_18__USP_S__P2000:
		buycommand += "buy glock; buy usp_silencer; buy hkp2000; ";
		break;
	case BuyPistols::DUAL_BERETTAS:
		buycommand += "buy elite; ";
		break;
	case BuyPistols::P250:
		buycommand += "buy p250; ";
		break;
	case BuyPistols::TEC_9__FIVE_SEVEN:
		buycommand += "buy tec9; buy fiveseven; ";
		break;
	case BuyPistols::DESERT_EAGLE__R8:
		buycommand += "buy deagle; buy revolver; ";
		break;
	}

	g_EngineClient->ExecuteClientCmd(buycommand.c_str());
}

void Misc::Clantag()
{
	static auto was_on = false;

	if (!Cfg.clantag) {
		if (was_on)
			Utilities::SetClantag("");
		was_on = false;
		return;
	}

	was_on = true;

	static int prevtime;
	static int prevval;

	if (int(g_GlobalVars->curtime * 2.9) != prevtime) {
		if (prevval != int(g_GlobalVars->curtime * 2.9) % 30) {
			prevval = int(g_GlobalVars->curtime * 2.9) % 30;
			switch (int(g_GlobalVars->curtime * 2.9) % 30)
			{
			case 29: { Utilities::SetClantag("[ killaura ]");  break; }
			case 28: { Utilities::SetClantag("[ killaur ]");  break; }
			case 27: { Utilities::SetClantag("[ killau ]");  break; }
			case 26: { Utilities::SetClantag("[ killa ]");  break; }
			case 25: { Utilities::SetClantag("[ kill ]");  break; }
			case 24: { Utilities::SetClantag("[ kil ]");  break; }
			case 23: { Utilities::SetClantag("[ ki ]");  break; }
			case 22: { Utilities::SetClantag("[ k ]");  break; }
			case 21: { Utilities::SetClantag("[ ki ]");  break; }
			case 20: { Utilities::SetClantag("[ kil ]");  break; }
			case 19: { Utilities::SetClantag("[ kill ]");  break; }
			case 18: { Utilities::SetClantag("[ killa ]");  break; }
			case 17: { Utilities::SetClantag("[ killau ]");  break; }
			case 16: { Utilities::SetClantag("[ killaur ]");  break; }
			case 15: { Utilities::SetClantag("[ killaura ]");  break; }
			case 14: { Utilities::SetClantag("[ killaur ]");  break; }
			case 13: { Utilities::SetClantag("[ killau ]");  break; }
			case 12: { Utilities::SetClantag("[ killa ]");  break; }
			case 11: { Utilities::SetClantag("[ kill ]");  break; }
			case 10: { Utilities::SetClantag("[ kil ]");  break; }
			case 9: { Utilities::SetClantag("[ ki ]");  break; }
			case 8: { Utilities::SetClantag("[ k ]");  break; }
			case 7: { Utilities::SetClantag("[ ki ]");  break; }
			case 6: { Utilities::SetClantag("[ kil ]");  break; }
			case 5: { Utilities::SetClantag("[ kill ]");  break; }
			case 4: { Utilities::SetClantag("[ killa ]");  break; }
			case 3: { Utilities::SetClantag("[ killau ]");  break; }
			case 2: { Utilities::SetClantag("[ killaur ]");  break; }
			case 1: { Utilities::SetClantag("[ killaura ]");  break; }
			default:;
			}
		}
	}
	prevtime = int(g_GlobalVars->curtime);
}

void Misc::ChatSpam()
{
	std::vector<std::string> chatspam = { "Killaura.host | Best private software" };
	static DWORD lastspammed = 0;
	if (GetTickCount64() - lastspammed > 800)
	{
		lastspammed = GetTickCount64();
		auto say = "say ";
		std::string msg = say + chatspam[rand() % chatspam.size()];
		g_EngineClient->ExecuteClientCmd(msg.c_str());
	}
}
void Misc::DisablePanoramaBlur()
{
	static auto blur = g_CVar->FindVar("@panorama_disable_blur");
	blur->SetValue(Cfg.disablepanoramablur);
}
void Misc::No3dSky()
{
	static auto sky = g_CVar->FindVar("r_3dsky");
	sky->SetValue(!Cfg.no3dsky);
}
void Misc::NoShadows()
{
	static ConVar* shadow = g_CVar->FindVar("cl_csm_enabled");
	static ConVar* shadow2 = g_CVar->FindVar("r_shadows");
	static ConVar* shadow3 = g_CVar->FindVar("cl_csm_static_prop_shadows");
	static ConVar* shadow4 = g_CVar->FindVar("cl_csm_shadows");
	static ConVar* shadow5 = g_CVar->FindVar("cl_csm_world_shadows");
	static ConVar* shadow6 = g_CVar->FindVar("cl_foot_contact_shadows");
	static ConVar* shadow7 = g_CVar->FindVar("cl_csm_viewmodel_shadows");
	static ConVar* shadow8 = g_CVar->FindVar("cl_csm_rope_shadows");
	static ConVar* shadow9 = g_CVar->FindVar("cl_csm_sprite_shadows");
	shadow->SetValue(!Cfg.noshadows);
	shadow2->SetValue(!Cfg.noshadows);
	shadow3->SetValue(!Cfg.noshadows);
	shadow4->SetValue(!Cfg.noshadows);
	shadow5->SetValue(!Cfg.noshadows);
	shadow6->SetValue(!Cfg.noshadows);
	shadow7->SetValue(!Cfg.noshadows);
	shadow8->SetValue(!Cfg.noshadows);
	shadow9->SetValue(!Cfg.noshadows);
}

void Misc::DisablePostProcessing()
{
	static ConVar* postprocessing = g_CVar->FindVar("mat_postprocess_enable");
	if (Cfg.noprocessing)
		postprocessing->SetValue(0);
	else
		postprocessing->SetValue(1);

}
void Misc::Nightmode()
{
	ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
	sv_skyname->m_nFlags &= ~(1 << 14);
	ConVar* r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;
		if (Cfg.misc_nightmode)
		{
			r_DrawSpecificStaticProp->SetValue(0);
			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(0.34f, 0.34f, 0.34f);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				sv_skyname->SetValue("sky_csgo_night02");
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
				pMaterial->ColorModulate(Cfg.misc_nightmode_value, Cfg.misc_nightmode_value, Cfg.misc_nightmode_value);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(0.3f, 0.3f, 0.3f);
			}
			if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
			{
				pMaterial->ColorModulate(228.f / 255.f, 35.f / 255.f, 157.f / 255.f);
			}
		}
		else if (!Cfg.misc_nightmode)
		{
			r_DrawSpecificStaticProp->SetValue(1);
			if (strstr(pMaterial->GetTextureGroupName(), "Model")) {
				pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				sv_skyname->SetValue("nukeblank");
				pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}
			if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
			{
				pMaterial->ColorModulate(1.f, 1.f, 1.f);
			}
		}
	}
	static auto ambient = g_CVar->FindVar("r_modelAmbientMin");
	static auto bloom = g_CVar->FindVar("mat_bloom_scalefactor_scalar");
	static auto bloom_scale = g_CVar->FindVar("mat_bloomamount_rate");
	if (Cfg.bloom)
	{
		bloom->SetValue(Cfg.bloom_value);
		ambient->SetValue(Cfg.model_ambient_value);
		bloom_scale->SetValue(5.05f / 255.f);
	}
	else if (!Cfg.bloom)
	{
		bloom->SetValue(0);
		ambient->SetValue(0);
	}
	if (Cfg.r_farz)
	{
		static const auto r_farz = g_CVar->FindVar("r_farz");
		r_farz->SetValue(Cfg.r_farz_value);//Все, что находится за пределами этого расстояния в мировых единицах измерения, не будет визуализировано
	}
	else if (!Cfg.r_farz)
	{
		static const auto r_farz = g_CVar->FindVar("r_farz");
		r_farz->SetValue(10000.f);
	}
}
void Misc::LegitAA(CUserCmd* cmd, bool bSendPacket)
{
	/*if (cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_USE) ||
		g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP ||
		!g_LocalPlayer->IsAlive())
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	short weapon_idx = weapon->m_Item().m_iItemDefinitionIndex();
	if ((weapon_idx == WEAPON_GLOCK || weapon_idx == WEAPON_FAMAS) && weapon->m_flNextPrimaryAttack() >= g_GlobalVars->curtime)
		return;


	if (weapon_idx == WEAPON_HEGRENADE ||
		weapon_idx == WEAPON_FRAG_GRENADE ||
		weapon_idx == WEAPON_INCGRENADE ||
		weapon_idx == WEAPON_SMOKEGRENADE ||
		weapon_idx == WEAPON_TAGRENADE ||
		weapon_idx == WEAPON_DECOY ||
		weapon_idx == WEAPON_FLASHBANG ||
		weapon_idx == WEAPON_MOLOTOV) { // hadrcoded aswell
		if (!weapon->m_bPinPulled() && weapon->m_fThrowTime() > 0.f)
			return;

		if (((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2)) && weapon->m_fThrowTime() > 0.f)
			return;
	}

	static float SpawnTime = 0.0f;
	if (g_LocalPlayer->m_flSpawnTime() != SpawnTime) {
		AnimState.pBaseEntity = g_LocalPlayer;
		g_LocalPlayer->ResetAnimationState(&AnimState);
		SpawnTime = g_LocalPlayer->m_flSpawnTime();
	}


	QAngle OldAngles = cmd->viewangles;

	static bool broke_lby = false;
	bool change_side = false;
	if (Cfg.legit_aa_flip) {
		change_side = true;
		Cfg.legit_aa_flip = false;
	}
	//side = -side;

	float minimal_move;
	bool should_move;
	if (change_side)
		side = -side;
	if (next_lby >= g_GlobalVars->curtime)
	{
		if (!broke_lby && *bSendPacket && g_ClientState->chokedcommands > 0)
			return;

		broke_lby = false;
		*bSendPacket = false;
		cmd->viewangles.yaw += 120.0f * side;
	}
	else
	{
		broke_lby = true;
		*bSendPacket = false;
		cmd->viewangles.yaw += 120.0f * -side;
	}

	Math::FixAngles(cmd->viewangles);
	Math::MovementFix(cmd, OldAngles, cmd->viewangles);

	cmd->viewangles.yaw = std::remainderf(cmd->viewangles.yaw, 360.f);

	static int latency_ticks = 0;
	float fl_latency = g_EngineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
	int latency = TIME_TO_TICKS(fl_latency);;
	if (g_ClientState->chokedcommands <= 0) {
		latency_ticks = latency;
	}
	else {
		latency_ticks = max(latency, latency_ticks);
	}

	int max_choke_ticks = 14;
	if (g_GameRules->m_bIsValveDS()) {
		if (fl_latency >= g_GlobalVars->interval_per_tick)
			max_choke_ticks = 11 - latency_ticks;
		else
			max_choke_ticks = 11;
	}
	else {
		max_choke_ticks = 13 - latency_ticks;
	}
	if (g_ClientState->chokedcommands >= max_choke_ticks) {
		*bSendPacket = true;
		cmd->viewangles = g_ClientState->viewangles;
	}

	auto anim_state = g_LocalPlayer->GetPlayerAnimState();
	if (anim_state) {
		CBasePlayerAnimState anim_state_backup = *anim_state;
		*anim_state = AnimState;
		*g_LocalPlayer->GetVAngles() = cmd->viewangles;
		g_LocalPlayer->UpdateClientSideAnimation();

		if (anim_state->speed_2d > 0.1f || std::fabsf(anim_state->flUpVelocity)) {
			next_lby = g_GlobalVars->curtime + 0.22f;
		}
		else if (g_GlobalVars->curtime > next_lby) {
			if (std::fabsf(Math::AngleDiff(anim_state->m_flGoalFeetYaw, anim_state->m_flEyeYaw)) > 35.0f) {
				next_lby = g_GlobalVars->curtime + 1.1f;
			}
		}

		AnimState = *anim_state;
		*anim_state = anim_state_backup;
	}

	if (*bSendPacket) {
		real_angle = AnimState.m_flGoalFeetYaw;
		view_angle = AnimState.m_flEyeYaw;
	}
	*/
	if (Cfg.legit_aa_type == 0)
		return;
	if (Cfg.legit_aa_type == 1)
	{
		static float last = 0.0f;
		static bool choke = false;
		if (g_GlobalVars->realtime >= last)
		{
			cmd->viewangles.yaw += 57.0f;
			bSendPacket = false;
			choke = true;
		}

		if (choke)
		{
			choke = true;
			bSendPacket = true;
			return;
		}
	}
	if (Cfg.legit_aa_type == 2)
	{
		static float last = 0.0f;
		static bool choke = false;
		cmd->viewangles.pitch += 90.0f;
		if (g_GlobalVars->realtime >= last)
		{
			cmd->viewangles.yaw += 120.0f;
			last = g_GlobalVars->realtime + 1.1f;
			bSendPacket = false;
			choke = true;
		}

		if (choke)
		{
			choke = false;
			bSendPacket = false;
			return;
		}
	}
}