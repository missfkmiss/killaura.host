#include "Visuals.hpp"
#include "../hooks.hpp"
#include "ragebot.hpp"
#include "wtypes.h"
#include <algorithm>
#pragma region Render
void VisualsRender::CreateFonts()
{
	VisualsFont = g_Surface->CreateFont_();
	g_Surface->SetFontGlyphSet(VisualsFont, "Tahoma", 12, 400, 0, 0, FONTFLAG_OUTLINE);

	EspFont = g_Surface->CreateFont_();
	g_Surface->SetFontGlyphSet(EspFont, "Visitor TT2 BRK", 10, 400, 0, 0, FONTFLAG_DROPSHADOW);

	watermark_ui_font = g_Surface->CreateFont_();
	g_Surface->SetFontGlyphSet(watermark_ui_font, "Verdana", 12, 400, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	small_pixel_11 = g_Surface->CreateFont_();
	g_Surface->SetFontGlyphSet(small_pixel_11, "Smallest Pixel-7", 11, 100, 0, 0, FONTFLAG_DROPSHADOW);

	verdanaBold_12 = g_Surface->CreateFont_();
	g_Surface->SetFontGlyphSet(verdanaBold_12, "Comic Sans", 24, 700, 0, 0, FONTFLAG_DROPSHADOW);
}

void Visuals::FireGameEvent(IGameEvent* event) {
	if (!strcmp(event->GetName(), "player_hurt")) {
		int attacker = event->GetInt("attacker");
		if (g_EngineClient->GetPlayerForUserID(attacker) == g_EngineClient->GetLocalPlayer()) {
			flHurtTime = g_GlobalVars->realtime;
		}
	}
	if (strcmp(event->GetName(), "player_death") == 0) {
		if (!event)
			return;

		auto entity = g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
		if (!entity)
			return;

		auto attacker = event->GetInt("attacker");
		if (!attacker)
			return;

		if (Cfg.kill_counter)
		{
			killtime = g_GlobalVars->realtime;
			kills++;
		}
		globalAlpha = g_GlobalVars->curtime;
	}

	if (!strcmp(event->GetName(), "round_start") && g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) {
		kills = 0;
	}
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_Surface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_Surface->DrawSetTextFont(font);
	g_Surface->DrawSetTextColor(r, g, b, a);
	g_Surface->DrawSetTextPos(x, y - height / 2);
	g_Surface->DrawPrintText(wbuf, wcslen(wbuf));
}

void Visuals::DrawString(unsigned long font, bool center, int x, int y, Color c, const char* fmt, ...)
{
	wchar_t* pszStringWide = reinterpret_cast<wchar_t*>(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
}

void Visuals::TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t* pszString)
{
	if (center)
	{
		int wide, tall;
		g_Surface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_Surface->DrawSetTextColor(c);
	g_Surface->DrawSetTextFont(font);
	g_Surface->DrawSetTextPos(x, y);
	g_Surface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
}

void VisualsRender::Text(int X, int Y, const char* Text, vgui::HFont Font, Color DrawColor, bool Center)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());

	g_Surface->DrawSetTextFont(Font);
	g_Surface->DrawSetTextColor(DrawColor);

	if (Center)
	{
		int TextWidth, TextHeight;
		VisualsRender::Get().TextSize(TextWidth, TextHeight, Text, Font);
		g_Surface->DrawSetTextPos(X - TextWidth / 2, Y);
	}
	else g_Surface->DrawSetTextPos(X, Y);
	g_Surface->DrawPrintText(WText.c_str(), wcslen(WText.c_str()));
}

void VisualsRender::TextSize(int& Width, int& Height, const char* Text, vgui::HFont Font)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());
	g_Surface->GetTextSize(Font, WText.c_str(), Width, Height);
}

void VisualsRender::FilledRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_Surface->DrawSetColor(DrawColor);
	g_Surface->DrawFilledRect(X1, Y1, X2, Y2);
}

void VisualsRender::OutlinedRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_Surface->DrawSetColor(DrawColor);
	g_Surface->DrawOutlinedRect(X1, Y1, X2, Y2);
}

void VisualsRender::Line(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_Surface->DrawSetColor(DrawColor);
	g_Surface->DrawLine(X1, Y1, X2, Y2);
}

void VisualsRender::Circle(int x, int y, int r, int seg, Color color)
{
	g_Surface->DrawSetColor(0, 0, 0, 255);
	g_Surface->DrawSetColor(color);
	g_Surface->DrawOutlinedCircle(x, y, r, seg);
}
#pragma endregion

RECT Visuals::GetBBox(C_BasePlayer* Player, Vector TransformedPoints[])
{
	RECT rect{};
	ICollideable* collideable = Player->GetCollideable();

	if (!collideable) return rect;

	Vector min = collideable->OBBMins();
	Vector max = collideable->OBBMaxs();

	const matrix3x4_t& trans = Player->m_rgflCoordinateFrame();

	Vector points[] =
	{
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

	Vector pos = Player->GetAbsOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
	{
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i])) return rect;

		TransformedPoints[i] = screen_points[i];
	}

	float left = screen_points[0].x;
	float top = screen_points[0].y;
	float right = screen_points[0].x;
	float bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x) left = screen_points[i].x;
		if (top < screen_points[i].y) top = screen_points[i].y;
		if (right < screen_points[i].x) right = screen_points[i].x;
		if (bottom > screen_points[i].y) bottom = screen_points[i].y;
	}
	return RECT{ LONG(left), LONG(top), LONG(right), LONG(bottom) };
}

bool Visuals::Begin(C_BasePlayer* Player)
{
	Context.Player = Player;
	if (!Context.Player->IsEnemy() || Context.Player->IsDormant())
		return false;

	Vector head = Context.Player->GetHitboxPos(HITBOX_HEAD);
	Vector origin = Context.Player->GetAbsOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, Context.HeadPos) || !Math::WorldToScreen(origin, Context.Origin)) return false;

	Vector points_transformed[8];
	RECT Box = GetBBox(Context.Player, points_transformed);

	Context.Box = Box;
	Context.Box.top = Box.bottom;
	Context.Box.bottom = Box.top;

	return true;
}

void Visuals::DrawLine(float x1, float y1, float x2, float y2, Color color, float size) {
	g_Surface->DrawSetColor(color);

	if (size == 1.f)
		g_Surface->DrawLine(x1, y1, x2, y2);
	else
		g_Surface->DrawFilledRect(x1 - (size / 2.f), y1 - (size / 2.f), x2 + (size / 2.f), y2 + (size / 2.f));
}

void Visuals::DrawBoxEdges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size) {
	if (fabs(x1 - x2) < (edge_size * 2)) {
		//x2 = x1 + fabs(x1 - x2);
		edge_size = fabs(x1 - x2) / 4.f;
	}


	DrawLine(x1, y1, x1, y1 + edge_size + (0.5f * edge_size), clr, size);
	DrawLine(x2, y1, x2, y1 + edge_size + (0.5f * edge_size), clr, size);
	DrawLine(x1, y2, x1, y2 - edge_size - (0.5f * edge_size), clr, size);
	DrawLine(x2, y2, x2, y2 - edge_size - (0.5f * edge_size), clr, size);
	DrawLine(x1, y1, x1 + edge_size, y1, clr, size);
	DrawLine(x2, y1, x2 - edge_size, y1, clr, size);
	DrawLine(x1, y2, x1 + edge_size, y2, clr, size);
	DrawLine(x2, y2, x2 - edge_size, y2, clr, size);
}

void Visuals::Box()
{
	static float rainbow1;
	rainbow1 += 0.0005;

	if (rainbow1 > 1.f)
		rainbow1 = 0.f;

	Color wmline2 = Color::FromHSB(rainbow1, 1.f, 1.f);
	if (Cfg.box_style == 1)
	{
		VisualsRender::Get().OutlinedRectange(Context.Box.left - 1, Context.Box.top - 1, Context.Box.right + 1, Context.Box.bottom + 1, Cfg.Rainbow ? Color(wmline2) : Color::Black);
		VisualsRender::Get().OutlinedRectange(Context.Box.left + 1, Context.Box.top + 1, Context.Box.right - 1, Context.Box.bottom - 1, Cfg.Rainbow ? Color(wmline2) : Color::Black);
		VisualsRender::Get().OutlinedRectange(Context.Box.left, Context.Box.top, Context.Box.right, Context.Box.bottom, Cfg.Rainbow ? Color(wmline2) : Color::White);
	}
	else if (Cfg.box_style == 2)
	{
		float edge_size = 25.f;
		if (Context.Player != g_LocalPlayer)
			edge_size = 4000.f / Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), Context.Player->m_vecOrigin());

		Visuals::Get().DrawBoxEdges(Context.Box.left, Context.Box.top, Context.Box.right, Context.Box.bottom, Cfg.Rainbow ? Color(wmline2) : Color(255, 255, 255, 255), edge_size, 1);
		Visuals::Get().DrawBoxEdges(Context.Box.left + 1, Context.Box.top + 1, Context.Box.right - 1, Context.Box.bottom - 1, Cfg.Rainbow ? Color(wmline2) : Color::Black, edge_size, 1);
		Visuals::Get().DrawBoxEdges(Context.Box.left - 1, Context.Box.top - 1, Context.Box.right + 1, Context.Box.bottom + 1, Cfg.Rainbow ? Color(wmline2) : Color::Black, edge_size, 1);

	}

}
void Visuals::Weapon()
{
	C_BaseCombatWeapon* Weapon = Context.Player->m_hActiveWeapon();
	if (!Weapon) return;
	std::string WeaponName = std::string(Weapon->GetCSWeaponData()->szHudName + std::string("(") + std::to_string(Weapon->m_iClip1()) + std::string("/") + std::to_string(Weapon->m_iPrimaryReserveAmmoCount()) + std::string(")"));
	WeaponName.erase(0, 13);
	int TextWidth, TextHeight;
	VisualsRender::Get().TextSize(TextWidth, TextHeight, WeaponName.c_str(), VisualsRender::Get().EspFont);
	VisualsRender::Get().Text(Context.Box.left + (Context.Box.right - Context.Box.left) / 2, Context.Box.bottom - 1, WeaponName.c_str(), VisualsRender::Get().EspFont, Color(255, 255, 255, 255), true);
}

void Visuals::Name()
{
	player_info_t PlayerInfo;
	g_EngineClient->GetPlayerInfo(Context.Player->EntIndex(), &PlayerInfo);

	int TextWidth, TextHeight;
	VisualsRender::Get().TextSize(TextWidth, TextHeight, PlayerInfo.szName, VisualsRender::Get().EspFont);
	VisualsRender::Get().Text(Context.Box.left + (Context.Box.right - Context.Box.left) / 2, Context.Box.top - TextHeight, PlayerInfo.szName, VisualsRender::Get().EspFont, Color(255, 255, 255, 255), true);
}

void Visuals::Skeleton()
{
	studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(Context.Player->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (Context.Player->SetupBonesS(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, Context.Player->m_flSimulationTime()))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t* bone = studioHdr->GetBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_Surface->DrawSetColor(Color(255, 255, 255, 255));
				g_Surface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}

void Visuals::DrawAngleLines()
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	//real
	Math::AngleVectors(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0), forward);
	src3D = g_LocalPlayer->m_vecOrigin();
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_Surface->DrawSetColor(Color::Red);
	g_Surface->DrawLine(src.x, src.y, dst.x, dst.y);

	//desync
	Math::AngleVectors(QAngle(0, g_LocalPlayer->EyeAngles().yaw, 0), forward);
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_Surface->DrawSetColor(Color::Green);
	g_Surface->DrawLine(src.x, src.y, dst.x, dst.y);
}

void Visuals::Recoil()
{
	if (!g_LocalPlayer->IsAlive()) return;

	int w, h;

	g_EngineClient->GetScreenSize(w, h);
	g_Surface->DrawSetColor(Color::White);

	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;

	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();

	x -= dx * int(punchAngle.yaw);
	y += dy * int(punchAngle.pitch);

	g_Surface->DrawLine(x - Cfg.Size, y, x + Cfg.Size + 1, y);
	g_Surface->DrawLine(x, y - Cfg.Size, x, y + Cfg.Size + 1);
}

void Visuals::Health()
{
	int HealthValue = Context.Player->m_iHealth();
	HealthValue = std::clamp(HealthValue, 0, 100);

	auto Multiplier = 12 / 360.f;
	Multiplier *= (HealthValue / 10.f) - 1;
	Color HealthColor = Color::FromHSB(Multiplier, 1, 1);

	float Height = (Context.Box.bottom - Context.Box.top) * float(HealthValue) / 100.f;

	VisualsRender::Get().FilledRectange(Context.Box.left - 7, Context.Box.top - 1, Context.Box.left - 2, Context.Box.bottom + 1, Color(0, 0, 0, 150));
	VisualsRender::Get().FilledRectange(Context.Box.left - 6, Context.Box.top, Context.Box.left - 3, Context.Box.top + Height, HealthColor);
}

void Visuals::Radar()
{
	for (auto i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		const auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!pEntity) continue;
		if (!pEntity->IsEnemy()) continue;

		pEntity->m_bSpotted() = true;
	}
}

void Visuals::ThirdPerson() {
	if (!g_LocalPlayer)
		return;

	if (Cfg.thirdperson && g_LocalPlayer->IsAlive()) {
		if (!g_Input->m_fCameraInThirdPerson) {
			g_Input->m_fCameraInThirdPerson = true;
		}

		float dist = Cfg.thirdperson_dist;

		QAngle* view = g_LocalPlayer->GetVAngles();
		trace_t tr;
		Ray_t ray;

		Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(-view->pitch)) * dist
		);

		//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
		ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
		CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

		float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK)  // If we are clear of obstacles
		{
			cameraDistance = dist; // go ahead and set the distance to the setting
		}
		else {
			if (vertOK) // if the Vertical Axis is OK
			{
				cameraDistance = distance2D * 0.95f;
			}
			else// otherwise we need to move closer to not go into the floor/ceiling
			{
				cameraDistance = abs(diff.z) * 0.95f;
			}
		}
		g_Input->m_fCameraInThirdPerson = true;

		g_Input->m_vecCameraOffset.z = cameraDistance;
	}
	else {
		g_Input->m_fCameraInThirdPerson = false;
	}
}

void Visuals::CustomFov()
{
	static auto customX = g_CVar->FindVar("viewmodel_offset_x");
	customX->m_fnChangeCallbacks.m_Size = 0;
	customX->SetValue(Cfg.xfov);

	static auto customY = g_CVar->FindVar("viewmodel_offset_y");
	customY->m_fnChangeCallbacks.m_Size = 0;
	customY->SetValue(Cfg.yfov);

	static auto customZ = g_CVar->FindVar("viewmodel_offset_z");
	customZ->m_fnChangeCallbacks.m_Size = 0;
	customZ->SetValue(Cfg.zfov);
}
void Visuals::Aspectratio()
{
	if (!Cfg.aspect_ratio)
		return;

	float ratio = (Cfg.aspect_ratio_value * 0.1) / 2;
	if (ratio > 0.001)
		g_CVar->FindVar("r_aspectratio")->SetValue(ratio);
	else
		g_CVar->FindVar("r_aspectratio")->SetValue((35 * 0.1f) / 2);

}

void Visuals::RemoveFog() {
	static auto fog_override = g_CVar->FindVar("fog_override");
	static auto fog_enable = g_CVar->FindVar("fog_enable");
	static auto fog_enable_water = g_CVar->FindVar("fog_enable_water_fog");
	if (Cfg.remove_fog) {
		fog_enable->SetValue(0);
		fog_override->SetValue(1);
		fog_enable_water->SetValue(0);
	}
	else {
		fog_enable->SetValue(1);
		fog_override->SetValue(0);
		fog_enable_water->SetValue(1);
	}
}

void Visuals::FullBright()
{
	static ConVar* fullbright = g_CVar->FindVar("mat_fullbright");
	if (Cfg.fullbright)
		fullbright->SetValue(1);
	else
		fullbright->SetValue(0);
}

void Visuals::DrawHitmarker() {
	/*
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	auto curtime = g_GlobalVars->realtime;
	float lineSize = 8.f;
	if (flHurtTime + .05f >= curtime) {
		int screenSizeX, screenCenterX;
		int screenSizeY, screenCenterY;
		g_EngineClient->GetScreenSize(screenSizeX, screenSizeY);

		screenCenterX = screenSizeX / 2;
		screenCenterY = screenSizeY / 2;

		Color bg = Color(0, 0, 0, 50);
		Color white = Color(255, 255, 255, 255);

		VisualsRender::Get().Line(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4), bg);
		VisualsRender::Get().Line(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4), bg);
		VisualsRender::Get().Line(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4), bg);
		VisualsRender::Get().Line(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4), bg);

		VisualsRender::Get().Line(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4), white);
		VisualsRender::Get().Line(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4), white);
		VisualsRender::Get().Line(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4), white);
		VisualsRender::Get().Line(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4), white);
	}
	*/
}

void Visuals::DrawFov()
{
	if (!Utilities::IsInGame())
		return;
	if (!g_LocalPlayer)
		return;
	if (!g_LocalPlayer->IsAlive())
		return;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

static auto linegoesthrusmoke = Utilities::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");
void Visuals::NoSmoke()
{
	if (!g_LocalPlayer || g_LocalPlayer->IsAlive())
		return;
	static bool set = true;
	static std::vector<const char*> smoke_materials =
	{
			"particle/beam_smoke_01",
			"particle/particle_smokegrenade",
			"particle/particle_smokegrenade1",
			"particle/particle_smokegrenade2",
			"particle/particle_smokegrenade3",
			"particle/particle_smokegrenade_sc",
			"particle/smoke1/smoke1",
			"particle/smoke1/smoke1_ash",
			"particle/smoke1/smoke1_nearcull",
			"particle/smoke1/smoke1_nearcull2",
			"particle/smoke1/smoke1_snow",
			"particle/smokesprites_0001",
			"particle/smokestack",
			"particle/vistasmokev1/vistasmokev1",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_nearcull",
			"particle/vistasmokev1/vistasmokev1_nearcull_fog",
			"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev4_emods_nocull",
			"particle/vistasmokev1/vistasmokev4_nearcull",
			"particle/vistasmokev1/vistasmokev4_nocull"
	};

	for (auto material_name : smoke_materials)
	{
		IMaterial* mat = g_MatSystem->FindMaterial(material_name, TEXTURE_GROUP_OTHER);
		mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Cfg.nosmoke);
	}
}

//-----------------------------------------------CHAMS------------------------------------------
Chams::Chams()
{
	std::ofstream("csgo\\materials\\material_textured.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture"	"vgui/white_additive"
  "$model"		"1"
  "$flat"			"0"
  "$nocull"		"1"
  "$halflambert"	"1"
  "$nofog"		"1"
  "$ignorez"		"0"
  "$znearer"		"0"
  "$wireframe"	"0"
}
)#";
	std::ofstream("csgo\\materials\\material_textured_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture"	"vgui/white_additive"
  "$model"		"1"
  "$flat"			"0"
  "$nocull"		"1"
  "$halflambert"	"1"
  "$nofog"		"1"
  "$ignorez"		"1"
  "$znearer"		"0"
  "$wireframe"	"0"
}
)#";
	std::ofstream("csgo\\materials\\material_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\material_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\models\\inventory_items\\trophy_majors\\gloss.vmt") << R"#("VertexLitGeneric" 
{
   "$basetexture" "vgui/white_additive"
   "$ignorez" "0"
   "$envmap" "env_cubemap"
   "$normalmapalphaenvmapmask" "1"
   "$envmapcontrast"  "1"
   "$nofog" "1"
   "$model" "1"
   "$nocull" "0"
   "$selfillum" "1"
   "$halflambert" "1"
   "$znearer" "0"
   "$flat" "1" 
}
)#";
	std::ofstream("csgo\\models\\inventory_items\\trophy_majors\\gloss_ignorez.vmt") << R"#("VertexLitGeneric" 
{
   "$basetexture" "vgui/white_additive"
   "$ignorez" "1"
   "$envmap" "env_cubemap"
   "$normalmapalphaenvmapmask" "1"
   "$envmapcontrast"  "1"
   "$nofog" "1"
   "$model" "1"
   "$nocull" "0"
   "$selfillum" "1"
   "$halflambert" "1"
   "$znearer" "0"
   "$flat" "1" 
}
)#";
	std::ofstream("csgo\\materials\\regular_reflective.vmt") << R"#("VertexLitGeneric" 
{
   "$basetexture" "vgui/white_additive"
   "$ignorez" "0"
   "$envmap" "env_cubemap"
   "$normalmapalphaenvmapmask" "1"
   "$envmapcontrast"  "1"
   "$nofog" "1"
   "$model" "1"
   "$nocull" "0"
   "$selfillum" "1"
   "$halflambert" "1"
   "$znearer" "0"
   "$flat" "1" 
}
)#";
	std::ofstream("csgo\\materials\\regular_reflective_ignorez.vmt") << R"#("VertexLitGeneric"
 {
   "$basetexture" "vgui/white_additive"
   "$ignorez" "1"
   "$envmap" "env_cubemap"
   "$normalmapalphaenvmapmask" "1"
   "$envmapcontrast"  "1"
   "$nofog" "1"
   "$model" "1"
   "$nocull" "0"
   "$selfillum" "1"
   "$halflambert" "1"
   "$znearer" "0"
   "$flat" "1" 
}
)#";
	std::ofstream("csgo/materials/glowOverlay.vmt") << R"#("VertexLitGeneric" 
{  
   	"$additive" "1"
   	"$envmap" "models/effects/cube_white"
   	"$envmaptint" "[1 1 1]"
   	"$envmapfresnel" "1"
   	"$envmapfresnelminmaxexp" "[0 1 2]"
   	"$alpha" "0.8"
 })#";

	std::ofstream("csgo\\materials\\simple_ignorez_reflective.vmt") << R"#("VertexLitGeneric"
{
		"$basetexture" "vgui/white_additive"
    "$ignorez"                       "1"
    "$envmap"              "env_cubemap"
    "$normalmapalphaenvmapmask"      "1"
    "$envmapcontrast"                "1"
    "$nofog"                         "1"
    "$model"                         "1"
    "$nocull"                        "0"
    "$selfillum"                     "1"
    "$halflambert"                   "1"
    "$znearer"                       "0"
    "$flat"                          "1"
    "$reflectivity"            "[1 1 1]"
    "$alpha"                         "1"
}
)#";

	std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
{
		"$basetexture" "vgui/white_additive"
    "$ignorez"                       "0"
    "$envmap"              "env_cubemap"
    "$normalmapalphaenvmapmask"      "1"
    "$envmapcontrast"                "1"
    "$nofog"                         "1"
    "$model"                         "1"
    "$nocull"                        "0"
    "$selfillum"                     "1"
    "$halflambert"                   "1"
    "$znearer"                       "0"
    "$flat"                          "1"
    "$reflectivity"            "[1 1 1]"
    "$alpha"                         "1"
}
)#";
	std::ofstream{ "csgo/materials/chamsAnimated.vmt" } <<
		"VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint \"[.7 .7 .7]\" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } }";

	std::ofstream("csgo/materials/skeetchams.vmt") << R"#("VertexLitGeneric"
{
	  "$basetexture" "models/effects/cube_white"
	  "$additive" "1"
	  "$envmap"  "models/effects/cube_white"
	  "$envmaptint"                "[1.0 1.0 1.0]"
	  "$envmapfresnel"            "1.0"
	  "$envmapfresnelminmaxexp"    "[0.0 1.0 2.0]"
	  "$alpha"                    "0.99"
}
)#";
	std::ofstream("csgo/materials/vibrance.vmt") << R"#("VertexLitGeneric" 
{
		"$basetexture" "VGUI/white_additive"
		"$bumpmap" "de_nuke/hr_nuke/pool_water_normals_002"
		"$bumptransform" "center 0.5 0.5 scale 0.25 0.25 rotate 0.0 translate 0.0 0.0"
		"$ignorez" "0"
		"$nofog" "1"
		"$model" "1"
		"$halflambert" "1"
		"$envmap" "env_cubemap"
		"$envmaptint" "[0.1 0.1 0.1]"
		"$envmapfresnel" "1.0"
		"$envmapfresnelminmaxexp" "[1.0, 1.0, 1.0]"
		"$phong" "1"
		"$phongexponent" "1024"
		"$phongboost" "4.0"
		"$phongfresnelranges" "[1.0, 1.0, 1.0]"
		"$rimlight" "1"
		"$rimlightexponent" "4.0"
		"$rimlightboost" "2.0"
    
		"Proxies"
		{
			"TextureScroll"
			{
				"textureScrollVar" "$bumptransform"
				"textureScrollRate" "0.25"
				"textureScrollAngle" "0.0"
			}
		}
})#";
	std::ofstream("csgo/materials/ghost.vmt") << R"#("VertexLitGeneric"
{
	"$baseTexture" "models\weapons\v_models\knife_flip\ghost\knife_flip_ghost"
	"$translucent" "1"
	"$selfillum"	"1"
	$additive 1
})#";

	materialRegular = g_MatSystem->FindMaterial("material_textured", TEXTURE_GROUP_MODEL);
	materialRegularIgnoreZ = g_MatSystem->FindMaterial("material_textured_ignorez", TEXTURE_GROUP_MODEL);
	materialFlat = g_MatSystem->FindMaterial("material_flat", TEXTURE_GROUP_MODEL);
	materialFlatIgnoreZ = g_MatSystem->FindMaterial("material_flat_ignorez", TEXTURE_GROUP_MODEL);
	materialCrystal = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_MODEL);
	materialMetallic = g_MatSystem->FindMaterial("simple_regular_reflective", TEXTURE_GROUP_MODEL);
	materialMetallicIgnoreZ = g_MatSystem->FindMaterial("simple_ignorez_reflective", TEXTURE_GROUP_MODEL);
	materialGlow = g_MatSystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_MODEL);//ясна глоу рифка xd
	materialGloss = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER);
	materialGlossIgnoreZ = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss_ignorez", TEXTURE_GROUP_OTHER);
	materialGlass = g_MatSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER);
	materialLightray = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags_lightray", TEXTURE_GROUP_OTHER);
	materialPlatinum = g_MatSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_MODEL);
	materialGlowChams = g_MatSystem->FindMaterial("glowOverlay", "Other models", TEXTURE_GROUP_MODEL);
	materialAnimated = g_MatSystem->FindMaterial("chamsAnimated", TEXTURE_GROUP_MODEL);
	materialVelvet = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_MODEL);
	materialPulse = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER);
	materialPalm = g_MatSystem->FindMaterial("models/props/de_dust/hr_dust/foliage/palm_bark_01", TEXTURE_GROUP_MODEL);
	materialGold = g_MatSystem->FindMaterial("models/inventory_items/wildfire_gold/wildfire_gold_detail", TEXTURE_GROUP_MODEL);
	materialGold2 = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_MODEL);
	materialGold3 = g_MatSystem->FindMaterial("models/inventory_items/vanguard_gold/vanguard_gold_detail", TEXTURE_GROUP_MODEL);
	materialMP3Detail = g_MatSystem->FindMaterial("models/inventory_items/music_kit/darude_01/mp3_detail", TEXTURE_GROUP_MODEL);
	materialSpeechInfo = g_MatSystem->FindMaterial("models/extras/speech_info", TEXTURE_GROUP_MODEL);
	materialESL_C = g_MatSystem->FindMaterial("models/weapons/customization/stickers/cologne2014/esl_c", TEXTURE_GROUP_MODEL);
	materialCharset = g_MatSystem->FindMaterial("models/inventory_items/contributor_map_tokens/contributor_charset_color", TEXTURE_GROUP_MODEL);
	materialDogtag = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags", TEXTURE_GROUP_MODEL);
	materialStarBlur = g_MatSystem->FindMaterial("models/inventory_items/dreamhack_trophies/dreamhack_star_blur", TEXTURE_GROUP_MODEL);
	materialCrystalDetail = g_MatSystem->FindMaterial("models/inventory_items/hydra_crystal/hydra_crystal_detail", TEXTURE_GROUP_MODEL);
	materialGlass2 = g_MatSystem->FindMaterial("models/inventory_items/service_medal_2015/glass", TEXTURE_GROUP_MODEL);
	materialSilver = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/silver_winners", TEXTURE_GROUP_MODEL);
	materialSkeet = g_MatSystem->FindMaterial("skeetchams", "Model textures");
	materialVibrance = g_MatSystem->FindMaterial("vibrance", "Model textures");
	materialGhost = g_MatSystem->FindMaterial("ghost", "Model textures");
}
Chams::~Chams()
{
	std::remove("csgo\\materials\\material_textured.vmt");
	std::remove("csgo\\materials\\material_textured_ignorez.vmt");
	std::remove("csgo\\materials\\material_flat.vmt");
	std::remove("csgo\\materials\\material_flat_ignorez.vmt");
	std::remove("csgo\\materials\\skeetchams.vmt");
	std::remove("csgo\\materials\\ghost.vmt");
	std::remove("csgo\\materials\\vibrance.vmt");
	std::remove("csgo\\materials\\chamsAnimated.vmt");
	std::remove("models/inventory_items/trophy_majors/crystal_clear.vmt");
	std::remove("dev/glow_armsrace.vmt");
	std::remove("models/inventory_items/trophy_majors/gloss.vmt");
	std::remove("models/inventory_items/trophy_majors/gloss_ignorez.vmt");
}
void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);
	const auto mdl = info.pModel;

	if (g_MdlRender->IsForcedMaterialOverride())
		return fnDME(g_MdlRender, 0, ctx, state, info, matrix);

	bool is_player = strstr(mdl->szName, "models/player");
	bool is_arm = strstr(mdl->szName, "arms");
	bool is_sleeve = strstr(mdl->szName, "arms");
	static IMaterial* normal = nullptr;
	static IMaterial* arm = nullptr;
	static IMaterial* backtrack = nullptr;
	static IMaterial* zbacktrack = nullptr;

	switch (Cfg.chams_material)
	{
	case 0:
		normal = materialRegular;
		break;
	case 1:
		normal = materialFlat;
		break;
	case 2:
		normal = materialCrystal;
		break;
	case 3:
		normal = materialGlow;
		break;
	case 4:
		normal = materialGloss;
		break;
	case 5:
		normal = materialGlass;
		break;
	case 6:
		normal = materialLightray;
		break;
	case 7:
		normal = materialMetallic;
		break;
	case 8:
		normal = materialPlatinum;
		break;
	case 9:
		normal = materialGlowChams;
		break;
	case 10:
		normal = materialAnimated;
		break;
	case 11:
		normal = materialVelvet;
		break;
	case 12:
		normal = materialPulse;
		break;
	case 13:
		normal = materialPalm;
		break;
	case 14:
		normal = materialGold;
		break;
	case 15:
		normal = materialGold2;
		break;
	case 16:
		normal = materialGold3;
		break;
	case 17:
		normal = materialMP3Detail;
		break;
	case 18:
		normal = materialSpeechInfo;
		break;
	case 19:
		normal = materialESL_C;
		break;
	case 20:
		normal = materialCharset;
		break;
	case 21:
		normal = materialDogtag;
		break;
	case 22:
		normal = materialStarBlur;
		break;
	case 23:
		normal = materialCrystalDetail;
		break;
	case 24:
		normal = materialGlass2;
		break;
	case 25:
		normal = materialSilver;
		break;
	case 26:
		normal = materialSkeet;
		break;
	case 27:
		normal = materialVibrance;
		break;
	case 28:
		normal = materialGhost;
		break;
	}

	switch (Cfg.backtrack_material)
	{
	case 0:
		backtrack = materialRegular;
		zbacktrack = materialRegularIgnoreZ;
		break;
	case 1:
		backtrack = materialFlat;
		zbacktrack = materialFlatIgnoreZ;
		break;
	case 2:
		backtrack = materialCrystal;
		zbacktrack = materialCrystal;
		break;
	case 3:
		backtrack = materialGlow;
		zbacktrack = materialGlow;
		break;
	case 4:
		backtrack = materialGloss;
		zbacktrack = materialGlossIgnoreZ;
		break;
	case 5:
		backtrack = materialGlass;
		zbacktrack = materialGlass;
		break;
	case 6:
		backtrack = materialLightray;
		zbacktrack = materialLightray;
		break;
	case 7:
		backtrack = materialMetallic;
		zbacktrack = materialMetallicIgnoreZ;
		break;
	case 8:
		backtrack = materialPlatinum;
		zbacktrack = materialPlatinum;
		break;
	case 9:
		backtrack = materialGlowChams;
		zbacktrack = materialGlowChams;
		break;
	case 10:
		backtrack = materialAnimated;
		zbacktrack = materialAnimated;
		break;
	case 11:
		backtrack = materialVelvet;
		zbacktrack = materialVelvet;
		break;
	case 12:
		backtrack = materialPulse;
		zbacktrack = materialPulse;
		break;
	case 13:
		backtrack = materialPalm;
		zbacktrack = materialPalm;
		break;
	case 14:
		backtrack = materialGold;
		zbacktrack = materialGold;
		break;
	case 15:
		backtrack = materialGold2;
		zbacktrack = materialGold2;
		break;
	case 16:
		backtrack = materialGold3;
		zbacktrack = materialGold3;
		break;
	case 17:
		backtrack = materialMP3Detail;
		zbacktrack = materialMP3Detail;
		break;
	case 18:
		backtrack = materialSpeechInfo;
		zbacktrack = materialSpeechInfo;
		break;
	case 19:
		backtrack = materialESL_C;
		zbacktrack = materialESL_C;
		break;
	case 20:
		backtrack = materialCharset;
		zbacktrack = materialCharset;
		break;
	case 21:
		backtrack = materialDogtag;
		zbacktrack = materialDogtag;
		break;
	case 22:
		backtrack = materialStarBlur;
		zbacktrack = materialStarBlur;
		break;
	case 23:
		backtrack = materialCrystalDetail;
		zbacktrack = materialCrystalDetail;
		break;
	case 24:
		backtrack = materialGlass2;
		zbacktrack = materialGlass2;
		break;
	case 25:
		backtrack = materialSilver;
		zbacktrack = materialSilver;
		break;
	case 26:
		backtrack = materialSkeet;
		zbacktrack = materialSkeet;
		break;
	case 27:
		backtrack = materialVibrance;
		zbacktrack = materialVibrance;
		break;
	case 28:
		backtrack = materialGhost;
		zbacktrack = materialGhost;
		break;
	}
	switch (Cfg.chams_arms_material)
	{
	case 0:
		arm = materialRegular;
		break;
	case 1:
		arm = materialFlat;
		break;
	case 2:
		arm = materialCrystal;
		break;
	case 3:
		arm = materialGlow;
		break;
	case 4:
		arm = materialGloss;
		break;
	case 5:
		arm = materialGlass;
		break;
	case 6:
		arm = materialLightray;
		break;
	case 7:
		arm = materialMetallic;
		break;
	case 8:
		arm = materialPlatinum;
		break;
	case 9:
		arm = materialGlowChams;
		break;
	case 10:
		arm = materialAnimated;
		break;
	case 11:
		arm = materialVelvet;
		break;
	case 12:
		arm = materialPulse;
		break;
	case 13:
		arm = materialPalm;
		break;
	case 14:
		arm = materialGold;
		break;
	case 15:
		arm = materialGold2;
		break;
	case 16:
		arm = materialGold3;
		break;
	case 17:
		arm = materialMP3Detail;
		break;
	case 18:
		arm = materialSpeechInfo;
		break;
	case 19:
		arm = materialESL_C;
		break;
	case 20:
		arm = materialCharset;
		break;
	case 21:
		arm = materialDogtag;
		break;
	case 22:
		arm = materialStarBlur;
		break;
	case 23:
		arm = materialCrystalDetail;
		break;
	case 24:
		arm = materialGlass2;
		break;
	case 25:
		arm = materialSilver;
		break;
	case 26:
		arm = materialSkeet;
		break;
	case 27:
		arm = materialVibrance;
		break;
	case 28:
		arm = materialGhost;
		break;
	}

	float color[3] = {
		Cfg.chams_color[0],
		Cfg.chams_color[1],
		Cfg.chams_color[2] };
	float zcolor[3] = {
		Cfg.chams_color_ignorez[0],
		Cfg.chams_color_ignorez[1],
		Cfg.chams_color_ignorez[2] };
	float backtrackcolor[3] = {
		Cfg.backtrack_color[0],
		Cfg.backtrack_color[1],
		Cfg.backtrack_color[2] };
	float armscolor[3] = {
		Cfg.chams_arms_color[0],
		Cfg.chams_arms_color[1],
		Cfg.chams_arms_color[2] };
	bool Found_normal = false;
	auto pVar_normal =	normal->FindVar("$envmaptint", &Found_normal);
	bool Found_arm = false;
	auto pVar_arm = arm->FindVar("$envmaptint", &Found_arm);
	if (Found_arm) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar_arm + 44))((uintptr_t)pVar_arm, Cfg.chams_arms_color[0], Cfg.chams_arms_color[1], Cfg.chams_arms_color[2]);
	bool Found_bt = false;
	auto pVar_bt = backtrack->FindVar("$envmaptint", &Found_bt);
	if (Found_bt) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar_bt + 44))((uintptr_t)pVar_bt, Cfg.backtrack_color[0], Cfg.backtrack_color[1], Cfg.backtrack_color[2]);
	if (is_player)
	{
		auto entity = C_BasePlayer::GetPlayerByIndex(info.entity_index);

		if (g_LocalPlayer && entity && entity->IsAlive())
		{
			if (entity->IsEnemy() && Cfg.chams_enable)
			{
				if (Cfg.RageEnabled && RageAimbot::Get().BacktrackRecords[info.entity_index].size() > 0)
				{
					switch (Cfg.chams_backtrack)
					{
					case 1:
						for (int t = 0; t < RageAimbot::Get().BacktrackRecords[info.entity_index].size(); t++)
						{
							if (!RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).MatrixBuilt
								|| !RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix)
								continue;
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(Cfg.backtrack_alpha);
							g_MdlRender->ForcedMaterialOverride(Cfg.chams_ignorez ? zbacktrack : backtrack);
							fnDME(g_MdlRender, 0, ctx, state, info, RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix);
						}
						break;
					case 2:
						if (RageAimbot::Get().BacktrackRecords[info.entity_index].back().MatrixBuilt
							&& RageAimbot::Get().BacktrackRecords[info.entity_index].back().BoneMatrix)
						{
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(Cfg.backtrack_alpha);
							g_MdlRender->ForcedMaterialOverride(Cfg.chams_ignorez ? zbacktrack : backtrack);
							fnDME(g_MdlRender, 0, ctx, state, info, RageAimbot::Get().BacktrackRecords[info.entity_index].back().BoneMatrix);
						}
						break;
					}
				}
				else if (Cfg.rage_backtrack && Backtrack::Get().BacktrackRecords[info.entity_index].size() > 0)
				{
					switch (Cfg.chams_backtrack)
					{
					case 1:
						for (int t = 0; t < Backtrack::Get().BacktrackRecords[info.entity_index].size(); t++)
						{
							if (!Backtrack::Get().BacktrackRecords[info.entity_index].at(t).MatrixBuilt
								|| !Backtrack::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix)
								continue;
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(Cfg.backtrack_alpha);
							g_MdlRender->ForcedMaterialOverride(Cfg.chams_ignorez ? zbacktrack : backtrack);
							fnDME(g_MdlRender, 0, ctx, state, info, Backtrack::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix);
						}
						break;
					case 2:
						if (Backtrack::Get().BacktrackRecords[info.entity_index].back().MatrixBuilt
							&& Backtrack::Get().BacktrackRecords[info.entity_index].back().BoneMatrix)
						{
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(Cfg.backtrack_alpha);
							g_MdlRender->ForcedMaterialOverride(Cfg.chams_ignorez ? zbacktrack : backtrack);
							fnDME(g_MdlRender, 0, ctx, state, info, Backtrack::Get().BacktrackRecords[info.entity_index].back().BoneMatrix);
						}
						break;
					}
				}
				normal->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Cfg.wireframe_mode);
				
				if (Cfg.chams_ignorez)
				{
					g_RenderView->SetColorModulation(zcolor);
					g_RenderView->SetBlend(Cfg.chams_alpha);
					if (!g_LocalPlayer->CanSeePlayer(entity, (HITBOX_HEAD || HITBOX_NECK || HITBOX_PELVIS || HITBOX_STOMACH || HITBOX_LOWER_CHEST || HITBOX_CHEST || HITBOX_UPPER_CHEST || HITBOX_RIGHT_THIGH || HITBOX_LEFT_THIGH || HITBOX_RIGHT_CALF || HITBOX_LEFT_CALF || HITBOX_RIGHT_FOOT || HITBOX_LEFT_FOOT || HITBOX_RIGHT_HAND || HITBOX_LEFT_HAND || HITBOX_RIGHT_UPPER_ARM || HITBOX_RIGHT_FOREARM || HITBOX_LEFT_UPPER_ARM || HITBOX_LEFT_FOREARM)))
					{
						g_RenderView->SetColorModulation(zcolor);
						if (Found_normal) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar_normal + 44))((uintptr_t)pVar_normal, Cfg.chams_color_ignorez[0], Cfg.chams_color_ignorez[1], Cfg.chams_color_ignorez[2]);
					}
					else if (g_LocalPlayer->CanSeePlayer(entity, (HITBOX_HEAD || HITBOX_NECK || HITBOX_PELVIS || HITBOX_STOMACH || HITBOX_LOWER_CHEST || HITBOX_CHEST || HITBOX_UPPER_CHEST || HITBOX_RIGHT_THIGH || HITBOX_LEFT_THIGH || HITBOX_RIGHT_CALF || HITBOX_LEFT_CALF || HITBOX_RIGHT_FOOT || HITBOX_LEFT_FOOT || HITBOX_RIGHT_HAND || HITBOX_LEFT_HAND || HITBOX_RIGHT_UPPER_ARM || HITBOX_RIGHT_FOREARM || HITBOX_LEFT_UPPER_ARM || HITBOX_LEFT_FOREARM)))
					{
						g_RenderView->SetColorModulation(color);
						if (Found_normal) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar_normal + 44))((uintptr_t)pVar_normal, Cfg.chams_color[0], Cfg.chams_color[1], Cfg.chams_color[2]);
					}
					normal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					g_MdlRender->ForcedMaterialOverride(normal);
					fnDME(g_MdlRender, 0, ctx, state, info, matrix);
				}
				else
				{
					g_RenderView->SetColorModulation(color);
					g_RenderView->SetBlend(Cfg.chams_alpha);
					normal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					if (Found_normal) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar_normal + 44))((uintptr_t)pVar_normal, Cfg.chams_color[0], Cfg.chams_color[1], Cfg.chams_color[2]);
					g_MdlRender->ForcedMaterialOverride(normal);
				}
			}
		}
	}
	else if (is_arm && Cfg.chams_arms_enable)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;
		g_RenderView->SetColorModulation(armscolor);
		
		g_RenderView->SetBlend(Cfg.chams_arms_alpha);
		g_MdlRender->ForcedMaterialOverride(arm);
	}
}
void Visuals::QuakeKillCounter()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected()) return;
	if (!Cfg.kill_counter)	return;
	if (!g_LocalPlayer)
		return;

	int screen_sizex, screen_sizey, cW, cH;
	g_EngineClient->GetScreenSize(screen_sizex, screen_sizey);

	cW = screen_sizex / 2;
	cH = screen_sizey / 2;

	float animationAlpha = 0.0;
	int alpha = 0;
	float animation_speed_value = 1.2;
	float step = 255 / 1.2 * g_GlobalVars->frametime;

	if (killtime + 0.8 > g_GlobalVars->realtime)
	{
		alpha = 255;
	}
	else
		alpha = alpha - step;

	float endTime = globalAlpha + 1.1f;
	float t = 1.0f - (endTime - g_GlobalVars->curtime) / (endTime - globalAlpha);

	float animationStep = animation_speed_value * g_GlobalVars->frametime;

	animationAlpha = animationAlpha - animationStep;

	screen_sizey = (screen_sizey / 2) - (t * (60.0f));

	if (alpha > 0) {
		switch (kills) {
		case 1:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(255, 0, 0, alpha), VisualsRender::Get().EspFont, true, "FIRST BLOOD");
			break;
		case 2:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(0, 0, 255, alpha), VisualsRender::Get().EspFont, true, "DOUBLE KILL");
			break;
		case 3:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(0, 255, 0, alpha), VisualsRender::Get().EspFont, true, "TRIPLE KILL");
			break;
		case 4:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(155, 200, 0, alpha), VisualsRender::Get().EspFont, true, "MULTI KILL");
			break;
		case 5:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(200, 0, 167, alpha), VisualsRender::Get().EspFont, true, "RAMPAGE");
			break;
		default:
			g_Surface->DrawT(screen_sizex / 2, screen_sizey, Color(23, 34, 255, alpha), VisualsRender::Get().EspFont, true, "DOMINATING");
			break;
		}
	}
}