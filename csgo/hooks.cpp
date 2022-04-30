#include "hooks.hpp"
#include <intrin.h>  
#include "helpers/input.hpp"
#include "options.hpp"
#include "helpers/Utilities.hpp"
#include "Features/Visuals.hpp"
#include "Features/ragebot.hpp"
#include "Features/Menu.hpp"
#include "helpers/math.hpp"
#include "Features/legitbot.h"
#include "features/misc.hpp"
#include "features/resolver.hpp"
#include <time.h>
#include "config.hpp"
#include <ShlObj_core.h>
#include "features/predictionsystem.h"
#define FRAMERATE_AVG_FRAC 0.9
#pragma intrinsic(_ReturnAddress) 

float side = 1.0f;

float real_angle = 0.0f;
float view_angle = 0.0f;

float Hooks::RealAngle;
float Hooks::FakeAngle;
float Hooks::Pitch;

bool overridething_dt_key;
int m_nTickbaseShift = 0;
std::string GetTimeString()
{
	time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}
int getfps()
{
	return static_cast<int>(1.f / g_GlobalVars->frametime);
}
int gettickrate()
{
	return static_cast<int>(1.f / g_GlobalVars->interval_per_tick);
}
int get_ping()
{
	auto net_channel = g_EngineClient->GetNetChannelInfo();
	return static_cast<int>(net_channel->GetLatency(FLOW_INCOMING) * 1000);
}

namespace Hooks
{
	vfunc_hook direct3d_hook;
	vfunc_hook hlclient_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook viewrender_hook;
	vfunc_hook enginehook_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook gameevents_hook;
	vfunc_hook prediction_hook;

	RecvPropHook* m_pEyeAngles_0;
	RecvPropHook* m_pEyeAngles_1;
	RecvPropHook* m_pFlashDuration;

	void Initialize()
	{
		direct3d_hook.setup(g_D3DDevice9);
		hlclient_hook.setup(g_CHLClient);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_Surface);
		mdlrender_hook.setup(g_MdlRender);
		enginehook_hook.setup(g_EngineClient);
		clientmode_hook.setup(g_ClientMode);
		gameevents_hook.setup(g_GameEvents);
		Prediction::Init();
		gameevents_hook.hook_index(index::FireEvent, hkFireEvent);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);


		prediction_hook.setup( g_Prediction );
		prediction_hook.hook_index( index::RunCommand, hkRunCommand );


		VisualsRender::Get().CreateFonts();

		m_pEyeAngles_0 = new RecvPropHook( C_BasePlayer::m_angEyeAngles( ) );
		m_pEyeAngles_1 = new RecvPropHook( C_BasePlayer::m_angEyeAngles_1( ) );
		m_pFlashDuration = new RecvPropHook( C_BasePlayer::m_flFlashDuration( ) );

		m_pEyeAngles_0->SetProxyFunction( Proxies::hkEyeAngles_0 );
		m_pEyeAngles_1->SetProxyFunction( Proxies::hkEyeAngles_1 );
		m_pFlashDuration->SetProxyFunction( Proxies::hkFlashDuration );

		g_InputSystem->EnableInput(true);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		Visuals::Get().NoSmoke();
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		if (Cfg.ambient_light)
		{
			mat_ambient_light_r->SetValue(Cfg.mat_ambient_light_r);
			mat_ambient_light_g->SetValue(Cfg.mat_ambient_light_g);
			mat_ambient_light_b->SetValue(Cfg.mat_ambient_light_b);
		}
		else if (!Cfg.ambient_light)
		{
			mat_ambient_light_r->SetValue(0.0f);
			mat_ambient_light_g->SetValue(0.0f);
			mat_ambient_light_b->SetValue(0.0f);
		}
		static auto misc_viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		misc_viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		misc_viewmodel_fov->SetValue(Cfg.viewmodel_fov);
		Misc::Get().Nightmode();
		//--------------------------------------------------------------------------------

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		Menu::Get().RenderMenu();

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
		{
			Menu::Get().OnDeviceReset();
			VisualsRender::Get().CreateFonts();
		}

		return hr;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		//bool* send_packet = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14);
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);
		QAngle OrgViewang;
		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);
		if (!cmd || !cmd->command_number) return;

		g_Globals.g_nTicks = cmd->tick_count;
		g_Globals.g_bProcessingPacket = true;

		if ( g_Globals.g_flVelocityModifier < 1.f )
			*reinterpret_cast< bool* >( ( uintptr_t ) g_Prediction + 0x24 ) = true;

		if ( g_ClientState->m_nDeltaTick > 0 )
			g_Prediction->Update( g_ClientState->m_nDeltaTick, true, g_ClientState->last_command_ack, g_ClientState->m_nLastOutgoingCmd + g_ClientState->m_nChockedCmds );

		if (g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame()) {
			Utilities::autoBhop(cmd);
			Utilities::AutoStrafe(cmd, cmd->viewangles);
		}

		Misc::Get().NoShadows();
		C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		static int ticks = 0;
		if (Cfg.FakelagChoke > 0) {
			if (g_LocalPlayer && g_LocalPlayer->IsAlive() && !(cmd->buttons & IN_ATTACK)) {
				if (ticks >= 15) {
					bSendPacket = true;
					ticks = 0;
				}
				else
					bSendPacket = ticks < 15 - Cfg.FakelagChoke;
				ticks++;
			}
		}
		//-----------------------------------

		if (cmd->buttons & IN_ATTACK)
			bSendPacket = true;

		/*if (Cfg.legit_enable) {
			LegitAimbot::Get().Aimbot(cmd, bSendPacket);
		}*/

		if (Cfg.InfiniteDuck) cmd->buttons |= IN_BULLRUSH;

		FakeLag::Get().FakeDuck(cmd, bSendPacket);
		if (Cfg.SlowWalk) {
			RageAimbot::Get().SlowWalk(cmd);
		}

		if (Cfg.buybot) {
			Misc::Get().OnCreateMove();
		}

		C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

		/*if (Cfg.autorevolver) {
			if (weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER) return;
			//if (Utilities::IsAbleToShoot()) return;
			static int delay = 0;
			delay--;
			if (delay <= 28) cmd->buttons |= IN_ATTACK;
			else delay = 0;
		}*/
		Prediction::StartPrediction(cmd, g_LocalPlayer); {
			MovementFix::Get().Start(cmd);
			if (Cfg.enable_legitbot && weapon)
			{
				legitbot::Get().do_aimbot(pLocal, weapon, cmd);
			}
			if (Cfg.RageEnabled) {
				RageAimbot::Get().StoreRecords();
				RageAimbot::Get().Do(cmd, Weapon, bSendPacket);
				if (Cfg.d_enable)
					RageAimbot::Get().Desync(cmd, bSendPacket);
				if (Cfg.AAEnabled)
					RageAimbot::Get().DoAntiaim(cmd, Weapon, bSendPacket);
			}
			if (Cfg.legit_desync)
				Misc::Get().LegitAA(cmd, &bSendPacket);
			MovementFix::Get().End(cmd);
		}
		Prediction::EndPrediction(g_LocalPlayer);
		static int since_use = 0;
		static int delay_shot = 0;
		static int shoot_state = 2;
		static int state_delay = 0;
		static bool doubletap = false;
		if (Cfg.doubletap)
		{
			doubletap = true;
			if (doubletap)
			{
				if (delay_shot != 0 && shoot_state == 0)
				{
					delay_shot--;
					cmd->buttons &= ~IN_ATTACK;
					since_use = 0;
					if (delay_shot == 0)
					{
						shoot_state = 2;
					}
				}
				if (cmd->buttons & IN_ATTACK && Cfg.doubletap_delay != 0 && Cfg.doubletap_shot != 0)
				{
					if (since_use++ < 3) {
						m_nTickbaseShift = 16;
						delay_shot = Cfg.doubletap_delay;
						shoot_state = 1;
						state_delay = Cfg.doubletap_shot;
					}
				}
				if (shoot_state == 1)
				{
					state_delay--;
					if (shoot_state == 1)
						cmd->buttons |= IN_ATTACK;
					if (state_delay == 0)
						shoot_state = 0;
				}
				if (state_delay <= 0)
					state_delay = 0;
				if (delay_shot <= 0)
					delay_shot = 0;
			}
		}
		else if (!Cfg.doubletap)
		{
			doubletap = false;
		}
		if (bSendPacket)
			Hooks::FakeAngle = cmd->viewangles.yaw;
		else
			Hooks::RealAngle = cmd->viewangles.yaw;

		Misc::Get().Clantag();

		static float next_lby = 0.0f;


		if (Cfg.chatspam) {
			Misc::Get().ChatSpam();
		}

		Hooks::Pitch = cmd->viewangles.pitch;

		Hooks::MoY_UgOl = cmd->viewangles;
		
		Math::Normalize(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);

		g_Globals.g_bProcessingPacket = false;

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
			push ebx
			lea  ecx, [esp]
			push ecx
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
	void DrawBeamPaw(Vector src, Vector end, Color color)
	{
		BeamInfo_t beamInfo;
		beamInfo.m_nType = 0; //TE_BEAMPOINTS
		beamInfo.m_vecStart = src;
		beamInfo.m_vecEnd = end;
		beamInfo.m_pszModelName = "sprites/glow01.vmt";
		beamInfo.m_pszHaloName = "sprites/glow01.vmt";
		beamInfo.m_flHaloScale = 3.0;
		beamInfo.m_flWidth = 4.5f;
		beamInfo.m_flEndWidth = 4.5f;
		beamInfo.m_flFadeLength = 0.5f;
		beamInfo.m_flAmplitude = 0;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.0f;
		beamInfo.m_nStartFrame = 0.0;
		beamInfo.m_flFrameRate = 0.0;
		beamInfo.m_flRed = color.r();
		beamInfo.m_flGreen = color.g();
		beamInfo.m_flBlue = color.b();
		beamInfo.m_nSegments = 1;
		beamInfo.m_bRenderable = true;
		beamInfo.m_flLife = 1;
		beamInfo.m_nFlags = 256 | 512 | 32768; //FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM
		Beam_t* myBeam = g_Beams->CreateBeamPoints(beamInfo);
		if (myBeam)
			g_Beams->DrawBeam(myBeam);
	}
	void AddTrails()
	{
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
		{
			static float rainbow;
			static Vector last_pos = Vector(0, 0, 0);

			rainbow += 0.0005;
			if (rainbow > 1.f)
				rainbow = 0.f;

			auto rainbow_col = Color::FromHSB(rainbow, 1, 1);
			auto local_pos = g_LocalPlayer->m_vecOrigin();
			DrawBeamPaw(last_pos, Vector(local_pos.x + Cfg.foottrails_xshift,
				local_pos.y + Cfg.foottrails_yshift, local_pos.z + Cfg.foottrails_zshift), rainbow_col);
			last_pos = local_pos;
		}
	}
	//--------------------------------------------------------------------------------
	void item_purchase(IGameEvent* pEvent) {

	}
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		if (Cfg.RemoveScope && strcmp("HudZoom", g_VGuiPanel->GetName(panel)) == 0) return;

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId)
		{
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel"))
			{
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{
			if (Cfg.foottrils)
				AddTrails();

			Visuals::Get().QuakeKillCounter();

			int ScreenWidth, ScreenHeight;
			g_EngineClient->GetScreenSize(ScreenWidth, ScreenHeight);

			if (Cfg.watermark)
			{
				int centerW, centerH;
				int w, h;
				g_EngineClient->GetScreenSize(w, h);
				centerW = w / 2;
				centerH = h / 2;
				
				g_Surface->DrawSetColor(Color(38, 38, 38, 150));
				g_Surface->DrawFilledRect((centerW * 2) - 247, 11, (centerW * 2) - 180 + 140, 11 + 24);
				g_Surface->DrawSetColor(Color(220, 0, 0, 150));
				g_Surface->DrawFilledRect((centerW * 2) - 246, 15, (centerW * 2) - 180 + 140, 16 + 2);

				Visuals::Get().DrawString(VisualsRender::Get().small_pixel_11, (centerW * 2) - 234, 24, Color(255, 255, 255), FONT_LEFT, ("killaura.host   |   %s   |"), GetTimeString().c_str());

				Visuals::Get().DrawString(VisualsRender::Get().small_pixel_11, (centerW * 2) - 122, 24, Color(255, 255, 255), FONT_LEFT, ("rate: %d   |"), gettickrate());

				Visuals::Get().DrawString(VisualsRender::Get().small_pixel_11, (centerW * 2) - 80, 24, Color(255, 255, 255), FONT_LEFT, (" fps: %d "), getfps());
			}

			if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer) return;

			C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

			if (Weapon && Cfg.RemoveScope && Weapon->IsSniper() && g_LocalPlayer->m_bIsScoped())
			{
				VisualsRender::Get().Line(ScreenWidth / 2, 0, ScreenWidth / 2, ScreenHeight, Color(0, 0, 0, 150));
				VisualsRender::Get().Line(0, ScreenHeight / 2, ScreenWidth, ScreenHeight / 2, Color(0, 0, 0, 150));
			}

			if (Cfg.Recoil) Visuals::Get().Recoil();

			if (g_LocalPlayer && InputSys::Get().IsKeyDown(VK_TAB) && Cfg.showranks)
				Utilities::RankRevealAll();

			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);

				if (!Player || !Player->IsAlive())
					continue;

				if (Visuals::Get().Begin(Player))
				{
					if (Cfg.Box) Visuals::Get().Box();
					if (Cfg.Health) Visuals::Get().Health();
					if (Cfg.Name) Visuals::Get().Name();
					if (Cfg.Weapon) Visuals::Get().Weapon();
					if (Cfg.Radar) Visuals::Get().Radar();
					if (Cfg.skeleton) Visuals::Get().Skeleton();
				}
			}

			if (Cfg.customfov)
				Visuals::Get().CustomFov();
			
			if (Cfg.aspect_ratio)
				Visuals::Get().Aspectratio();
			if (Cfg.hitmarker)
				Visuals::Get().DrawHitmarker();
			Visuals::Get().RemoveFog();
			Visuals::Get().FullBright();
			Visuals::Get().DrawFov();
			if (g_LocalPlayer->IsAlive()) {
				if (Cfg.anglelines)
					Visuals::Get().DrawAngleLines();
			}
		}
	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);
		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void AgentChanger(int v_char, int p_id)
	{
		const char* models_to_change[] =
		{
		XorStr("models/player/custom_player/legacy/tm_phoenix.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantj.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantg.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_varianti.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_varianti.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantm.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantg.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variante.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantk.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_varianth.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_varianth.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variantg.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_variantg.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_varianth.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_varianti.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_varianth.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variantg.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/tm_anarchist.mdl"),
		XorStr("models/player/custom_player/legacy/tm_anarchist_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_anarchist_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_anarchist_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_anarchist_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/tm_pirate.mdl"),
		XorStr("models/player/custom_player/legacy/tm_pirate_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_pirate_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_pirate_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_pirate_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/tm_professional.mdl"),
		XorStr("models/player/custom_player/legacy/tm_professional_var1.mdl"),
		XorStr("models/player/custom_player/legacy/tm_professional_var2.mdl"),
		XorStr("models/player/custom_player/legacy/tm_professional_var3.mdl"),
		XorStr("models/player/custom_player/legacy/tm_professional_var4.mdl"),
		XorStr("models/player/custom_player/legacy/tm_separatist.mdl"),
		XorStr("models/player/custom_player/legacy/tm_separatist_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_separatist_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_separatist_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_separatist_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gign.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gign_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gign_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gign_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gign_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gsg9.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gsg9_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gsg9_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gsg9_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_gsg9_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf_variante.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_idf_variantf.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_swat.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_swat_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_swat_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_swat_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_swat_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_st6_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variante.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_balkan_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_jumpsuit_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_jumpsuit_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_heavy.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_heavy.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/tm_leet_variante.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_variantb.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/tm_phoenix_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_varianta.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variantc.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variantd.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_fbi_variante.mdl"),
		XorStr("models/player/custom_player/legacy/ctm_sas.mdl") };

		auto entity = (C_BaseEntity*)g_EntityList->GetClientEntity(p_id);

		if (!entity)
			return;

		if (entity->IsPlayer()) {
			auto ModelIndex = g_MdlInfo->GetModelIndex(models_to_change[v_char]);

			if (ModelIndex)
				entity->SetModelIndex(ModelIndex);
		}


	}
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);
		if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer)
		{
			ofunc(g_CHLClient, edx, stage); return;
		}
		if (stage == FRAME_RENDER_START)
		{
			Misc::Get().DisablePanoramaBlur();
			Misc::Get().No3dSky();
			Misc::Get().DisablePostProcessing();
			for (int i = 1; i <= 64; i++)
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);
				if (!Player || !Player->IsPlayer() || Player == g_LocalPlayer) continue;

				*(int*)((uintptr_t)Player + 0xA30) = g_GlobalVars->framecount;
				*(int*)((uintptr_t)Player + 0xA28) = 0;
			}

			QAngle aim_punch_old;
			QAngle view_punch_old;

			QAngle* aim_punch = nullptr;
			QAngle* view_punch = nullptr;

			if (Cfg.novisualrecoil)
			{
				aim_punch = &g_LocalPlayer->m_aimPunchAngle();
				view_punch = &g_LocalPlayer->m_viewPunchAngle();

				aim_punch_old = *aim_punch;
				view_punch_old = *view_punch;

				*aim_punch = QAngle(0.f, 0.f, 0.f);
				*view_punch = QAngle(0.f, 0.f, 0.f);
			}

			if (g_EngineClient->IsConnected() && g_LocalPlayer->IsAlive()) {
				if (g_Input->m_fCameraInThirdPerson)
					*g_LocalPlayer->GetVAngles() = Hooks::MoY_UgOl;
			}
		}
		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		{
			for (int i = 1; i <= 64; i++)
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);
				if (!Player || !Player->IsAlive()) continue;
				if (Player->IsDormant()) continue;

				auto varmap = reinterpret_cast<uintptr_t>(Player) + 36;
				auto varmap_size = *reinterpret_cast<int*>(varmap + 20);

				for (auto index = 0; index < varmap_size; index++)*
					reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(varmap) + index * 12) = 0;
			}
			if (Cfg.resolver)
				Resolver::Get().Resolve();
		}
		if (Cfg.misc_model) {
			if (g_LocalPlayer->m_iTeamNum() == 3)
				AgentChanger(Cfg.misc_model_type_ct, g_EngineClient->GetLocalPlayer());
			if (g_LocalPlayer->m_iTeamNum() == 2)
				AgentChanger(Cfg.misc_model_type_tt, g_EngineClient->GetLocalPlayer());
		}

		static int m_iLastCmdAck = 0;
		static float m_flNextCmdTime = 0.f;
		if ( g_ClientState && ( m_iLastCmdAck != g_ClientState->last_command_ack || m_flNextCmdTime != g_ClientState->m_flNextCmdTime ) )
		{
			if ( g_Globals.g_flVelocityModifier != g_LocalPlayer->m_flVelocityModifier( ) )
			{
				*( bool* )( ( uintptr_t ) g_Prediction + 0x24 ) = true;
				g_Globals.g_flVelocityModifier = g_LocalPlayer->m_flVelocityModifier( );
			}

			m_iLastCmdAck = g_ClientState->last_command_ack;
			m_flNextCmdTime = g_ClientState->m_flNextCmdTime;
		}

		ofunc(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible())
		{
			g_Surface->UnlockCursor();
			return;
		}
		ofunc(g_Surface);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		if (InputSys::Get().IsKeyDown(Cfg.FakeDuck_Bind))
		{
			vsView->origin.z = g_LocalPlayer->m_vecOrigin().z + 64.0f;
		}

		if (g_EngineClient->IsInGame() && vsView) {
			if (Cfg.thirdperson_bind && InputSys::Get().WasKeyPressed(Cfg.thirdperson_bind))
				Cfg.thirdperson = !Cfg.thirdperson;

			Visuals::Get().ThirdPerson();
		}

		ofunc(g_ClientMode, edx, vsView);
	}

	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(21);
		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
		ofunc(g_MdlRender, 0, ctx, state, pInfo, pCustomBoneToWorld);
		g_MdlRender->ForcedMaterialOverride(nullptr);
		
	}

	bool __stdcall hkFireEvent(IGameEvent* pEvent)
	{
		static auto oFireEvent = gameevents_hook.get_original<FireEvent>(index::FireEvent);
		Visuals::Get().FireGameEvent(pEvent);
		if (strstr(pEvent->GetName(), "player_hurt")) {
			auto
				userid = pEvent->GetInt("userid"),
				attacker = pEvent->GetInt("attacker");
			if (userid || attacker) {
				auto
					userid_id = g_EngineClient->GetPlayerForUserID(userid),
					attacker_id = g_EngineClient->GetPlayerForUserID(attacker);
				player_info_t userid_info, attacker_info;
				if (g_EngineClient->GetPlayerInfo(userid_id, &userid_info) || g_EngineClient->GetPlayerInfo(attacker_id, &attacker_info))
				{
					std::stringstream ss;
					if (attacker_id == g_EngineClient->GetLocalPlayer()) {
						if (Cfg.damage_show) {
							ss << " \x01[\x0CKILLAURA\x01] You hurt " << userid_info.szName << " for " << pEvent->GetInt("dmg_health") << "hp";
							g_ChatElement->ChatPrintf(g_EngineClient->GetLocalPlayer(), 0, ss.str().c_str());
						}
						if (Cfg.hitsound) {
							g_Surface->PlaySound_("buttons\\arena_switch_press_02.wav");
						}
						if (Cfg.hiteffect) {
							g_LocalPlayer->m_flHealthShotBoostExpirationTime() = g_GlobalVars->curtime + 1.f;
						}
					}
				}
			}
		}

		return oFireEvent(g_GameEvents, pEvent);
	}

	void __fastcall hkRunCommand( IPrediction* m_pPrediction, uint32_t, C_BasePlayer* m_pPlayer, CUserCmd* m_pCmd, IMoveHelper* m_pMoveHelper )
	{
		static auto oFunc = prediction_hook.get_original< RunCommand_t >( index::RunCommand );

		if ( !m_pPlayer || m_pPlayer != g_LocalPlayer )
			return oFunc( m_pPrediction, m_pPlayer, m_pCmd, m_pMoveHelper );

		if ( g_Globals.g_iTickrate + g_Globals.g_nTicks + 8 <= m_pCmd->tick_count )
		{
			m_pCmd->hasbeenpredicted = true;
			return;
		}

		float m_flVelocityModifier = g_LocalPlayer->m_flVelocityModifier( );

		if ( g_Globals.g_bProcessingPacket && ( m_pCmd->command_number == g_ClientState->last_command_ack + 1 ) )
			m_pPlayer->m_flVelocityModifier( ) = g_Globals.g_flVelocityModifier;

		oFunc( m_pPrediction, m_pPlayer, m_pCmd, m_pMoveHelper );

		if ( !g_Globals.g_bProcessingPacket )
			m_pPlayer->m_flVelocityModifier( ) = m_flVelocityModifier;
	}
}
	//--------------------------------------------------------------------------------

void Hooks::Proxies::hkEyeAngles_0( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult )
{
	static auto m_pOrig = m_pEyeAngles_0->GetOriginalFunction( );
	m_pOrig( m_pData, m_pStruct, m_pResult );
}

void Hooks::Proxies::hkEyeAngles_1( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult )
{
	static auto m_pOrig = m_pEyeAngles_1->GetOriginalFunction( );
	m_pOrig( m_pData, m_pStruct, m_pResult );
}

void Hooks::Proxies::hkFlashDuration( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult )
{
	static auto m_pOrig = m_pFlashDuration->GetOriginalFunction( );

	if ( Cfg.noflash )
		*reinterpret_cast< float* >( m_pResult ) = 0.f;
	else
		m_pOrig( m_pData, m_pStruct, m_pResult );
}

void WriteUserCmd(void* buffer, CUserCmd* in, CUserCmd* out)
{
	static DWORD WriteUsercmdF = (DWORD)Utilities::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

	__asm
	{
		mov ecx, buffer
		mov edx, in
		push out
		call WriteUsercmdF
		add  esp, 4
	}
}

bool __fastcall Hooks::hkWriteUsercmdDeltaToBuffer(void* m_pClient, void*, int m_nSlot, void* m_pBuffer, int m_nFrom, int m_nTo, bool m_bNewCmd)
{
	static auto ofunc = hlclient_hook.get_original<decltype(&hkWriteUsercmdDeltaToBuffer)>(24);

	if (m_nTickbaseShift >= 0)
		return ofunc(g_ClientState, m_pClient, m_nSlot, m_pBuffer, m_nFrom, m_nTo, m_bNewCmd);

	if (m_nFrom != -1)
		return true;
	int m_nTickbase = m_nTickbaseShift;
	m_nTickbaseShift = 0;

	int* m_pnNewCmds = (int*)((uintptr_t)m_pBuffer - 0x2C);
	int* m_pnBackupCmds = (int*)((uintptr_t)m_pBuffer - 0x30);

	*m_pnBackupCmds = 0;

	int m_nNewCmds = *m_pnNewCmds;
	int m_nNextCmd = g_ClientState->m_nChockedCmds + g_ClientState->m_nLastOutgoingCmd + 1;
	int m_nTotalNewCmds = std::min(m_nNewCmds + abs(m_nTickbase), 62);

	*m_pnNewCmds = m_nTotalNewCmds;

	for (m_nTo = m_nNextCmd - m_nNewCmds + 1; m_nTo <= m_nNextCmd; m_nTo++)
	{
		if (!ofunc(g_ClientState, m_pClient, m_nSlot, m_pBuffer, m_nFrom, m_nTo, true))
			return false;

		m_nFrom = m_nTo;
	}

	CUserCmd* m_pCmd = g_Input->GetUserCmd(m_nSlot, m_nFrom);
	if (!m_pCmd)
		return true;

	CUserCmd m_ToCmd = *m_pCmd, m_FromCmd = *m_pCmd;
	m_ToCmd.command_number++;
	m_ToCmd.tick_count += 3 * g_GlobalVars->tickcount;
	
	for (int i = 0; i <= abs(m_nTickbase); i++)
	{
		WriteUserCmd(m_pBuffer, &m_ToCmd, &m_FromCmd);
		m_FromCmd = m_ToCmd;

		m_ToCmd.command_number++;
		m_ToCmd.tick_count++;
	}

	return true;
}

