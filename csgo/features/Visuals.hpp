#pragma once
#include "../options.hpp"
#include "../valve_sdk/structs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/Utilities.hpp"
#include "../singleton.hpp"

enum FontRenderFlag_t
{
	FONT_LEFT = 0,
	FONT_RIGHT = 1,
	FONT_CENTER = 2
};

class VisualsRender : public Singleton<VisualsRender>
{
	friend class Singleton<VisualsRender>;
private:
public:
	vgui::HFont VisualsFont;
	vgui::HFont EspFont;
	vgui::HFont watermark_ui_font;
	vgui::HFont small_pixel_11;
	vgui::HFont verdanaBold_12;

	void CreateFonts();
	void Text(int X, int Y, const char* Text, vgui::HFont Font, Color DrawColor, bool Center);
	void TextSize(int& Width, int& Height, const char* Text, vgui::HFont Font);
	void FilledRectange(int X1, int Y1, int X2, int Y2, Color DrawColor);
	void OutlinedRectange(int X1, int Y1, int X2, int Y2, Color DrawColor);
	void Line(int X1, int Y1, int X2, int Y2, Color DrawColor);
	void Circle(int x, int y, int r, int seg, Color color);
};

class Visuals : public Singleton<Visuals>
{
	float flHurtTime;
	int kills = 0; 
	float killtime = 0.0f;
	int globalAlpha = 0;
	friend class Singleton<Visuals>;
private:
	RECT GetBBox(C_BasePlayer* Player, Vector TransformedPoints[]);
	struct
	{
		C_BasePlayer* Player;
		RECT Box;
		Vector HeadPos;
		Vector FeetPos;
		Vector Origin;
		Color Color;
	} Context;
public:
	virtual void FireGameEvent(IGameEvent* event);

	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);
	void DrawString(unsigned long font, bool center, int x, int y, Color c, const char* fmt, ...);
	void TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t* pszString);

	bool Begin(C_BasePlayer* Player);
	void DrawLine(float x1, float y1, float x2, float y2, Color color, float size = 1.f);
	void DrawBoxEdges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size = 1.f);
	void DrawAngleLines();
	void Health();
	void Name();
	void Skeleton();
	void Box();
	void Weapon();
	void Radar();
	void Recoil();
	void ThirdPerson();
	void CustomFov();
	void Aspectratio();
	void RemoveFog();
	void FullBright();
	void DrawHitmarker();
	void DrawFov();
	void NoSmoke();
	void QuakeKillCounter();
};

class Chams
	: public Singleton<Chams>
{
	friend class Singleton<Chams>;
	Chams();
	~Chams();

public:
	void OnDrawModelExecute(
		IMatRenderContext* ctx,
		const DrawModelState_t& state,
		const ModelRenderInfo_t& pInfo,
		matrix3x4_t* pCustomBoneToWorld);
private:
	IMaterial* materialRegular = nullptr;
	IMaterial* materialRegularIgnoreZ = nullptr;
	IMaterial* materialFlat = nullptr;
	IMaterial* materialFlatIgnoreZ = nullptr;
	IMaterial* materialCrystal = nullptr;
	IMaterial* materialGlow = nullptr;
	IMaterial* materialGloss = nullptr;
	IMaterial* materialGlossIgnoreZ = nullptr;
	IMaterial* materialMetallic = nullptr;
	IMaterial* materialMetallicIgnoreZ = nullptr;
	IMaterial* materialGlass = nullptr;
	IMaterial* materialLightray = nullptr;
	IMaterial* materialPlatinum = nullptr;
	IMaterial* materialGlowChams = nullptr;
	IMaterial* materialAnimated = nullptr;
	IMaterial* materialVelvet = nullptr;
	IMaterial* materialPulse = nullptr;
	IMaterial* materialPalm = nullptr;
	IMaterial* materialGold = nullptr;
	IMaterial* materialGold2 = nullptr;
	IMaterial* materialGold3 = nullptr;
	IMaterial* materialMP3Detail = nullptr;
	IMaterial* materialSpeechInfo = nullptr;
	IMaterial* materialESL_C = nullptr;
	IMaterial* materialCharset = nullptr;
	IMaterial* materialDogtag = nullptr;
	IMaterial* materialStarBlur = nullptr;
	IMaterial* materialCrystalDetail = nullptr;
	IMaterial* materialGlass2 = nullptr;
	IMaterial* materialSilver = nullptr;
	IMaterial* materialSkeet = nullptr;
	IMaterial* materialVibrance = nullptr;
	IMaterial* materialGhost = nullptr;
};