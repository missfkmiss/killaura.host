#define NOMINMAX
#define IMGUI_DEFINE_MATH_OPERATORS

#include "menu.hpp"
#include <Windows.h>
#include <chrono>
#include "..//menuarrays.hpp"
#include "visuals.hpp"
#include "../config.hpp"
#include "../menuarrays.hpp"
#include "misc.hpp"
#include "../font/iconfont.h"
using namespace std;

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif

#define RAGE XorStr("A")
#define AIMBOT XorStr("I") 
#define VISUALS XorStr("J")
#define SKINS XorStr("B")
#define MISC XorStr("G")

ImFont* Text;
ImFont* WeaponText;
ImFont* forTabs = nullptr;
ImFont* forTabs_text = nullptr;
ImFont* Main;

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	static bool ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
			{
				*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
				return true;
			}, &lambda, items_count, height_in_items);
	}

	IMGUI_API  bool TabLabels(int numTabs, const char** tabLabels, int& selectedIndex, const char** tabLabelTooltips, bool autoLayout, int* pOptionalHoveredIndex) {
		ImGuiStyle& style = ImGui::GetStyle();

		const ImVec4 color = style.Colors[ImGuiCol_Button];
		const ImVec4 Text = style.Colors[ImGuiCol_Text];
		const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
		const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
		const ImVec2 itemSpacing = style.ItemSpacing;
		style.ItemSpacing.x = 2.5;
		style.ItemSpacing.y = 1;
		if (numTabs > 0 && (selectedIndex < 0 || selectedIndex >= numTabs)) selectedIndex = 0;
		if (pOptionalHoveredIndex) *pOptionalHoveredIndex = -1;

		const float btnOffset = 2.f * style.FramePadding.x;
		const float sameLineOffset = 2.f * style.ItemSpacing.x;
		const float uniqueLineOffset = 2.f * style.WindowPadding.x;

		float windowWidth = 0.f, sumX = 0.f;
		if (autoLayout) windowWidth = ImGui::GetWindowWidth() - uniqueLineOffset;

		bool selection_changed = false;
		for (int i = 0; i < numTabs; i++)
		{
			if (i == selectedIndex)
			{
				style.Colors[ImGuiCol_Text] = ImColor(87, 48, 161, 255);
				style.Colors[ImGuiCol_Button] = colorActive;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorActive;
			}
			else
			{
				style.Colors[ImGuiCol_Text] = Text;
				style.Colors[ImGuiCol_Button] = color;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorHover;
			}

			ImGui::PushID(i);

			if (!autoLayout) { if (i > 0) ImGui::SameLine(); }
			else if (sumX > 0.f) {
				sumX += sameLineOffset;
				sumX += ImGui::CalcTextSize(tabLabels[i]).x + btnOffset;
				if (sumX > windowWidth) sumX = 0.f;
				else ImGui::SameLine();
			}
			if (ImGui::Button(tabLabels[i], ImVec2((windowWidth / numTabs) - 2, 20))) { selection_changed = (selectedIndex != i); selectedIndex = i; }
			if (autoLayout && sumX == 0.f) {
				sumX = ImGui::GetItemRectSize().x;
			}
			if (pOptionalHoveredIndex) {
				if (ImGui::IsItemHovered()) {
					*pOptionalHoveredIndex = i;
					if (tabLabelTooltips && tabLabelTooltips[i] && strlen(tabLabelTooltips[i]) > 0)  ImGui::SetTooltip("%s", tabLabelTooltips[i]);
				}
			}
			else if (tabLabelTooltips && tabLabelTooltips[i] && ImGui::IsItemHovered() && strlen(tabLabelTooltips[i]) > 0) ImGui::SetTooltip("%s", tabLabelTooltips[i]);
			ImGui::PopID();
		}

		style.Colors[ImGuiCol_Text] = Text;
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		style.ItemSpacing = itemSpacing;
		return selection_changed;
	}

	const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
	};

	static bool IsKeyPressedMap(ImGuiKey key, bool repeat = true)
	{
		const int key_index = GImGui->IO.KeyMap[key];
		return (key_index >= 0) ? ImGui::IsKeyPressed(key_index, repeat) : false;
	}

	bool ImGui::Hotkey(const char* label, int* k, const ImVec2& size_arg)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
		const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f), window->DC.CursorPos + size);
		const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id, false);
		const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
		const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

		const bool hovered = ImGui::ItemHoverable(frame_bb, id);

		if (hovered) {
			ImGui::SetHoveredID(id);
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}

		const bool user_clicked = hovered && io.MouseClicked[0];

		if (focus_requested || user_clicked) {
			if (g.ActiveId != id) {
				// Start edition
				memset(io.MouseDown, 0, sizeof(io.MouseDown));
				memset(io.KeysDown, 0, sizeof(io.KeysDown));
				*k = 0;
			}
			ImGui::SetActiveID(id, window);
			ImGui::FocusWindow(window);
		}
		else if (io.MouseClicked[0]) {
			// Release focus when we click outside
			if (g.ActiveId == id)
				ImGui::ClearActiveID();
		}

		bool value_changed = false;
		int key = *k;

		if (g.ActiveId == id) {
			for (auto i = 0; i < 5; i++) {
				if (io.MouseDown[i]) {
					switch (i) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
			if (!value_changed) {
				for (auto i = VK_BACK; i <= VK_RMENU; i++) {
					if (io.KeysDown[i]) {
						key = i;
						value_changed = true;
						ImGui::ClearActiveID();
					}
				}
			}

			if (IsKeyPressedMap(ImGuiKey_Escape)) {
				*k = 0;
				ImGui::ClearActiveID();
			}
			else {
				*k = key;
			}
		}

		// Render
		// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

		char buf_display[64] = "None";

		ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

		if (*k != 0 && g.ActiveId != id) {
			strcpy_s(buf_display, KeyNames[*k]);
		}
		else if (g.ActiveId == id) {
			strcpy_s(buf_display, "<Press a key>");
		}

		const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
		ImVec2 render_pos = frame_bb.Min + style.FramePadding;
		ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);

		if (label_size.x > 0)
			ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

		return value_changed;
	}

}

void Menu::Initialize()
{
	Visible = false;

	ImGui::CreateContext();
	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	CreateStyle();

	ImGuiIO& io = ImGui::GetIO();
	Text = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(menufont, sizeof(menufont), 15.0f);
	WeaponText = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(weaponfont, sizeof(weaponfont), 20.0f);
	forTabs_text = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Ubuntu_compressed_data, Ubuntu_compressed_size, 15, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	forTabs = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(IconsForTabs_compressed_data, IconsForTabs_compressed_size, 25, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	Main = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Shutdown() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#pragma region button
void BtnNormal()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImColor(86, 87, 81, 255);
}
void BtnActive()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImColor(87, 48, 161, 255);
}
void BtnInviz()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0, 0);

}
void BtnNormalMenu()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(15, 15, 15, 255);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(17, 17, 17, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(17, 17, 17, 255);
}
#pragma endregion
#pragma region Tab
bool TabEx(const char* label, const char* icon, const bool selected, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	if (selected)
		window->DrawList->AddRectFilled({ bb.Min.x + 20,bb.Max.y - 7 }, { bb.Max.x - 20,bb.Max.y - 5 }, ImColor(255, 255, 255), 6);

	window->DrawList->AddText(ImVec2(bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(255, 255, 255, 255), label);

	return pressed;
}

bool Tab(const char* label, const char* icon, const ImVec2& size_arg, const bool selected)
{
	return TabEx(label, icon, selected, size_arg);
}

#pragma endregion

void Menu::RenderMenu()
{
	ImGui::GetIO().MouseDrawCursor = Visible;

	auto& style = ImGui::GetStyle();

	if (!Visible)
		return;

	static auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

	ImGui_ImplDX9_NewFrame();

	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

	if (Visible)
	{
		if (style.Alpha > 1.f)
			style.Alpha = 1.f;
		else if (style.Alpha != 1.f)
			style.Alpha += 0.01f;
	}
	else
	{
		if (style.Alpha < 0.f)
			style.Alpha = 0.f;
		else if (style.Alpha != 0.f)
			style.Alpha -= 0.02f;
	}

	if (&Visible)
	{
		DWORD flags123 = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowPos(ImVec2(5, 5));
		ImGui::SetNextWindowSize(ImVec2(180, 5));
		ImGui::Begin("hello", &Visible, flags123);
		{
			ImGui::TextColored(ImColor(87, 48, 161), "Welcome back, dear User!");
		}ImGui::End();
	}

	static int MenuTab = 0;
	ImGui::PushFont(Text);

	ImGui::SetNextWindowSize({ 660,500 });
	if (ImGui::Begin(" Killaura.host", &Visible, flags)) {
		if (ImGui::BeginChild("Child1", ImVec2(-1, 25), false, flags)) {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(Cfg.color_menu[0], Cfg.color_menu[1], Cfg.color_menu[2]));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + -3), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15));
			ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + 2), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(0, 0, 0, 150));

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("killaura.host").x) / 2);
			ImGui::TextColored(ImColor(255, 255, 255), "killaura.host");
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("Child2", ImVec2(-1, 50), false, flags)) {

			if (Tab("LegitBot", "", { 100,50 }, MenuTab == 0 ? true : false))
				MenuTab = 0;

			ImGui::SameLine();

			if (Tab("RageBot", "", { 100,50 }, MenuTab == 1 ? true : false))
				MenuTab = 1;

			ImGui::SameLine();

			if (Tab("AntiHit", "", { 100,50 }, MenuTab == 2 ? true : false))
				MenuTab = 2;

			ImGui::SameLine();

			if (Tab("Visuals", "", { 100,50 }, MenuTab == 3 ? true : false))
				MenuTab = 3;

			ImGui::SameLine();

			if (Tab("Skins", "", { 100,50 }, MenuTab == 4 ? true : false))
				MenuTab = 4;

			ImGui::SameLine();

			if (Tab("Misc", "", { 100,50 }, MenuTab == 5 ? true : false))
				MenuTab = 5;

		}
		ImGui::EndChild();

		if (ImGui::BeginChild("Child3", ImVec2(730, -1), false, flags)) {
			if (MenuTab == 0) {
				ImGui::BeginChild("##firstchild", ImVec2(0, 0));
				{
					static int active_wlegit_tab = 0;
					ImGui::BeginChild("##weaponselect", ImVec2(-1, 50));
					{
						ImGui::PopFont();
						ImGui::PushFont(WeaponText);

						if (Tab("d", "", { 100,50 }, active_wlegit_tab == 0 ? true : false))
							active_wlegit_tab = 0;

						ImGui::SameLine();

						if (Tab("e", "", { 100,50 }, active_wlegit_tab == 1 ? true : false))
							active_wlegit_tab = 1;

						ImGui::SameLine();

						if (Tab("F", "", { 100,50 }, active_wlegit_tab == 2 ? true : false))
							active_wlegit_tab = 2;

						ImGui::SameLine();

						if (Tab("D", "", { 100,50 }, active_wlegit_tab == 3 ? true : false))
							active_wlegit_tab = 3;

						ImGui::SameLine();

						if (Tab("g", "", { 100,50 }, active_wlegit_tab == 4 ? true : false))
							active_wlegit_tab = 4;

						ImGui::PopFont();
						ImGui::PushFont(Text);
					}
					ImGui::EndChild();

					ImGui::BeginChild("##generaloption", ImVec2(325, 0));
					{
						ImGui::Text("[General]");
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::Checkbox("Enable##legit", &Cfg.enable_legitbot);
						ImGui::Checkbox("Aim on key", &Cfg.aimonkey);
						if (Cfg.aimonkey)
						{
							ImGui::Text("Key");
							ImGui::Hotkey("##keyyyy", &Cfg.aimkey);
							ImGui::Spacing();
						}
						ImGui::SliderInt("##Randomize smooth", &Cfg.randomize_smooth, 0, 2, "Randomize smooth : %.0f");

						ImGui::Text("Randomize angle");
						ImGui::Combo("##typeangle", &Cfg.randomize_angle, randomize_angle, ARRAYSIZE(randomize_angle));

					}
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("##weaponoption", ImVec2(0, 0));
					{
						if (active_wlegit_tab == 0) {
							ImGui::Text("[Aimbot pistol]");
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SliderFloat("##Maximum Fov", &Cfg.legit_fov_pistols, 1.0f, 20.0f, "Maximum Fov : %.1f", 0.5f);
							ImGui::SliderFloat("##Speed", &Cfg.legit_smooth_pistols, 1.0f, 20.0f, "Speed : %.1f", 0.5f);
							ImGui::SliderFloat("##Recoilmax", &Cfg.legit_rcsmax_pistols, 1.0f, 100.0f, "Recoil max : %.1f");
							ImGui::SliderFloat("##Recoilmin", &Cfg.legit_rcsmin_pistols, 1.0f, 100.0f, "Recoil min : %.1f");
							ImGui::SliderInt("##Hitbox", &Cfg.hitbox_pistols, 1, 7, "Hitbox : %.0f");
						}

						else if (active_wlegit_tab == 1) {
							ImGui::Text("[Aimbot rifle]");
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SliderFloat("##Maximum Fov", &Cfg.legit_fov_rifles, 1.0f, 20.0f, "Maximum Fov : %.1f", 0.5f);
							ImGui::SliderFloat("##Speed", &Cfg.legit_smooth_rifles, 1.0f, 20.0f, "Speed : %.1f", 0.5f);
							ImGui::SliderFloat("##Recoilmax", &Cfg.legit_rcsmax_rifles, 1.0f, 100.0f, "Recoil max : %.1f");
							ImGui::SliderFloat("##Recoilmin", &Cfg.legit_rcsmin_rifles, 1.0f, 100.0f, "Recoil min : %.1f");
							ImGui::SliderInt("##Hitbox", &Cfg.hitbox_rifles, 1, 7, "Hitbox : %.0f");
						}

						else if (active_wlegit_tab == 2) {
							ImGui::Text("[Aimbot scout]");
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SliderFloat("##Maximum Fov", &Cfg.legit_fov_scout, 1.0f, 20.0f, "Maximum Fov : %.1f", 0.5f);
							ImGui::SliderFloat("##Speed", &Cfg.legit_smooth_scout, 1.0f, 20.0f, "Speed : %.1f", 0.5f);
							ImGui::SliderFloat("##Recoilmax", &Cfg.legit_rcsmax_scout, 1.0f, 100.0f, "Recoil max : %.1f");
							ImGui::SliderFloat("##Recoilmin", &Cfg.legit_rcsmin_scout, 1.0f, 100.0f, "Recoil min : %.1f");
							ImGui::SliderInt("##Hitbox", &Cfg.hitbox_scout, 1, 7, "Hitbox : %.0f");
						}

						else if (active_wlegit_tab == 3) {
							ImGui::Text("[Aimbot auto]");
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SliderFloat("##Maximum Fov", &Cfg.legit_fov_auto, 1.0f, 20.0f, "Maximum Fov : %.1f", 0.5f);
							ImGui::SliderFloat("##Speed", &Cfg.legit_smooth_auto, 1.0f, 20.0f, "Speed : %.1f", 0.5f);
							ImGui::SliderFloat("##Recoilmax", &Cfg.legit_rcsmax_auto, 1.0f, 100.0f, "Recoil max : %.1f");
							ImGui::SliderFloat("##Recoilmin", &Cfg.legit_rcsmin_auto, 1.0f, 100.0f, "Recoil min : %.1f");
							ImGui::SliderInt("##Hitbox", &Cfg.hitbox_auto, 1, 7, "Hitbox : %.0f");
						}

						else if (active_wlegit_tab == 4) {
							ImGui::Text("[Aimbot awp]");
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SliderFloat("##Maximum Fov", &Cfg.legit_fov_awp, 1.0f, 20.0f, "Maximum Fov : %.1f", 0.5f);
							ImGui::SliderFloat("##Speed", &Cfg.legit_smooth_awp, 1.0f, 20.0f, "Speed : %.1f", 0.5f);
							ImGui::SliderFloat("##Recoilmax", &Cfg.legit_rcsmax_awp, 1.0f, 100.0f, "Recoil max : %.1f");
							ImGui::SliderFloat("##Recoilmin", &Cfg.legit_rcsmin_awp, 1.0f, 100.0f, "Recoil min : %.1f");
							ImGui::SliderInt("##Hitbox", &Cfg.hitbox_awp, 1, 7, "Hitbox : %.0f");
						}

					}
					ImGui::EndChild();

				}
				ImGui::EndChild();
			}

			if (MenuTab == 1) {
				static int active_rage_tab = 0;

				if (Tab("General", "", { 100,50 }, active_rage_tab == 0 ? true : false))
					active_rage_tab = 0;

				ImGui::SameLine();

				if (Tab("Weapon", "", { 100,50 }, active_rage_tab == 2 ? true : false))
					active_rage_tab = 1;

				if (active_rage_tab == 0)
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::BeginChild("##firstchild", ImVec2(0, 0));
					{
						ImGui::Checkbox("Enable", &Cfg.RageEnabled);
						ImGui::Checkbox("Auto scope", &Cfg.AutoScope);
						ImGui::Checkbox("Auto crouch", &Cfg.autocrouch);
						ImGui::Checkbox("Auto stop", &Cfg.autostop);
						/*	if (Cfg.autostop)
							{
								ImGui::Combo("Mode##astope", &Cfg.autostop_mode, autostop_mode, ARRAYSIZE(autostop_mode));
							}*/
						ImGui::Text("[Body aim]");
						ImGui::Checkbox("On air", &Cfg.baim_onair);
						ImGui::Checkbox("When slowwalking", &Cfg.baim_whenslowwalk);
						ImGui::Checkbox("On key", &Cfg.baim_onkey);
						if (Cfg.baim_onkey)
							ImGui::Hotkey("BAim key", &Cfg.baim_onkey_key);
						ImGui::Checkbox("Doubletap", &Cfg.doubletap);
						ImGui::Text("Delay shot");
						ImGui::SliderInt("##delay-shot", &Cfg.doubletap_shot, 0, 30, "%.f");
						ImGui::Text("State delay");
						ImGui::SliderInt("##state-delay-shot", &Cfg.doubletap_delay, 0, 20);
					}
					ImGui::EndChild();

					ImGui::NextColumn();

					ImGui::BeginChild("##secondchild", ImVec2(0, 0));
					{
						ImGui::Text("[Hitscan]");
						ImGui::Checkbox("Enable", &Cfg.hitscan_enable);
						if (Cfg.hitscan_enable) {
							ImGui::Checkbox("Head", &Cfg.rage_hitscan1);
							ImGui::Checkbox("Neck", &Cfg.rage_hitscan2);
							ImGui::Checkbox("Chest", &Cfg.rage_hitscan3);
							ImGui::Checkbox("Stomach", &Cfg.rage_hitscan4);
							ImGui::Checkbox("Pelvis", &Cfg.rage_hitscan5);
							ImGui::Checkbox("Arms", &Cfg.rage_hitscan6);
							ImGui::Checkbox("Legs", &Cfg.rage_hitscan7);
						}
						ImGui::Checkbox("Backtrack##rage", &Cfg.rage_backtrack);
						if (Cfg.rage_backtrack)
							ImGui::SliderInt("##Timebacktracktimerage", &Cfg.rage_backtrack_time, 1, 200, "Time : %.f");
					}
					ImGui::EndChild();
				}
				else if (active_rage_tab == 1)
				{
					static int active_wrage_tab = 0;

					ImGui::PopFont();
					ImGui::PushFont(WeaponText);

					if (Tab("d", "", { 100,50 }, active_wrage_tab == 0 ? true : false))
						active_wrage_tab = 0;

					ImGui::SameLine();

					if (Tab("R", "", { 100,50 }, active_wrage_tab == 1 ? true : false))
						active_wrage_tab = 1;

					ImGui::SameLine();

					if (Tab("e", "", { 100,50 }, active_wrage_tab == 2 ? true : false))
						active_wrage_tab = 2;

					ImGui::SameLine();

					if (Tab("F", "", { 100,50 }, active_wrage_tab == 3 ? true : false))
						active_wrage_tab = 3;

					ImGui::SameLine();

					if (Tab("D", "", { 100,50 }, active_wrage_tab == 4 ? true : false))
						active_wrage_tab = 4;

					ImGui::SameLine();

					if (Tab("g", "", { 100,50 }, active_wrage_tab == 5 ? true : false))
						active_wrage_tab = 5;

					ImGui::PopFont();
					ImGui::PushFont(Text);

					ImGui::Spacing();

					if (active_wrage_tab == 0) {
						ImGui::SliderInt("##Hitchancer0", &Cfg.PistolHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr0", &Cfg.PistolDamage, 0, 100, "Min Damage : %.f");
					}

					else if (active_wrage_tab == 1) {
						ImGui::SliderInt("##Hitchancer1", &Cfg.HeavyPistHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr1", &Cfg.HeavyPistDamage, 0, 100, "Min Damage : %.f");
					}

					else if (active_wrage_tab == 2) {
						ImGui::SliderInt("##Hitchancer2", &Cfg.RifleHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr2", &Cfg.RifleDamage, 0, 100, "Min Damage : %.f");
					}

					else if (active_wrage_tab == 3) {
						ImGui::SliderInt("##Hitchancer3", &Cfg.ScoutHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr3", &Cfg.ScoutDamage, 0, 100, "Min Damage : %.f");
					}

					else if (active_wrage_tab == 4) {
						ImGui::SliderInt("##Hitchancer4", &Cfg.AutoHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr4", &Cfg.AutoDamage, 0, 100, "Min Damage : %.f");
					}

					else if (active_wrage_tab == 5) {
						ImGui::SliderInt("##Hitchancer5", &Cfg.AwpHitchance, 0, 100, "Hitchance : %.f");
						ImGui::SliderInt("##Min Dmgr5", &Cfg.AwpDamage, 0, 100, "Min Damage : %.f");
					}
				}
			}

			if (MenuTab == 2) {
				ImGui::Columns(3, nullptr, false);
				ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
					if (Cfg.AAEnabled) {
						ImGui::Text("[Standing]");
						ImGui::Combo("Pitch##stand", &Cfg.pitch_standing, pitchText, ARRAYSIZE(pitchText));
						if (Cfg.pitch_standing == 5)
							ImGui::SliderInt("Pitch value", &Cfg.aapitch_cust_stand, -100.f, 100.f);
						ImGui::Combo("Yaw##stand", &Cfg.yaw_standing, yawText, ARRAYSIZE(yawText));
						if (Cfg.yaw_standing == 2)
							ImGui::SliderInt("##Jitterradiusstand", &Cfg.jitter_radius_stand, 10, 90, "Jitter radius : %.f");
						ImGui::SliderInt("##stand", &Cfg.yawadd_stand, -180, 180, "Yaw add : %.f");
						if (Cfg.yaw_standing == 3) {
							ImGui::Hotkey("Left side", &Cfg.AALeftSideBind_stand);
							ImGui::Hotkey("Back side", &Cfg.AABackSideBind_stand);
							ImGui::Hotkey("Right side", &Cfg.AARightSideBind_stand);
						}
					}
					ImGui::Spacing();
					ImGui::Text("[Misc]");
					ImGui::Checkbox("Enable AA##aaenable", &Cfg.AAEnabled);
					ImGui::Checkbox("Desync Jitter", &Cfg.desync_jitter);
					if (!Cfg.desync_with_max_delta) ImGui::SliderInt("##maxdelta", &Cfg.desync_delta, 10, 58, " %.0f");
					ImGui::Checkbox("Use max delta", &Cfg.desync_with_max_delta);
					ImGui::Checkbox("Desync", &Cfg.d_enable);
					ImGui::Checkbox("Resolver", &Cfg.resolver);
					/*if (Cfg.d_enable)
					{
						//ImGui::Hotkey("Desync flip key", &Cfg.d_flipkey);
						ImGui::Combo("Type", &Cfg.rage_desync_type, desyncType, ARRAYSIZE(desyncType));
					}*/

				}
				ImGui::EndChild();

				ImGui::NextColumn();

				ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {
					if (Cfg.AAEnabled) {
						ImGui::Text("[Moving]");
						ImGui::Combo("Pitch##move", &Cfg.pitch_moving, pitchText, ARRAYSIZE(pitchText));
						if (Cfg.pitch_moving == 5)
							ImGui::SliderInt("Pitch value", &Cfg.aapitch_cust_mov, -100.f, 100.f);
						ImGui::Combo("Yaw##move", &Cfg.yaw_moving, yawText, ARRAYSIZE(yawText));
						if (Cfg.yaw_moving == 2)
							ImGui::SliderInt("##Jitterradiusmov", &Cfg.jitter_radius_mov, 10, 90, "Jitter radius : %.f");
						ImGui::SliderInt("##mov", &Cfg.yawadd_mov, -180, 180, "Yaw add : %.f");
						if (Cfg.yaw_moving == 3) {
							ImGui::Hotkey("Left side", &Cfg.AALeftSideBind_mov);
							ImGui::Hotkey("Back side", &Cfg.AABackSideBind_mov);
							ImGui::Hotkey("Right side", &Cfg.AARightSideBind_mov);
						}

					}
				}
				ImGui::EndChild();

				ImGui::NextColumn();

				ImGui::BeginChild("##thirdchild", ImVec2(0, 0)); {
					if (Cfg.AAEnabled)
					{
						ImGui::Text("[Air]");
						ImGui::Combo("Pitch##air", &Cfg.pitch_air, pitchText, ARRAYSIZE(pitchText));
						if (Cfg.pitch_air == 5)
							ImGui::SliderInt("Pitch value", &Cfg.aapitch_cust_air, -100.f, 100.f);
						ImGui::Combo("Yaw##air", &Cfg.yaw_air, yawText, ARRAYSIZE(yawText));
						if (Cfg.yaw_air == 2)
							ImGui::SliderInt("##Jitterradiusair", &Cfg.jitter_radius_air, 10, 90, "Jitter radius : %.f");
						ImGui::SliderInt("##air", &Cfg.yawadd_air, -180, 180, "Yaw add : %.f");
						if (Cfg.yaw_air == 3)
						{
							ImGui::Hotkey("Left side", &Cfg.AALeftSideBind_air);
							ImGui::Hotkey("Back side", &Cfg.AABackSideBind_air);
							ImGui::Hotkey("Right side", &Cfg.AARightSideBind_air);
						}
					}

				}
				ImGui::EndChild();
			}

			if (MenuTab == 3) {
				static int active_visuals_tab = 0;

				if (Tab("Esp", "", { 100,50 }, active_visuals_tab == 0 ? true : false))
					active_visuals_tab = 0;

				ImGui::SameLine();

				if (Tab("Misc", "", { 100,50 }, active_visuals_tab == 1 ? true : false))
					active_visuals_tab = 1;

				ImGui::SameLine();

				if (Tab("Chams", "", { 100,50 }, active_visuals_tab == 2 ? true : false))
					active_visuals_tab = 2;

				if (active_visuals_tab == 0)
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::BeginChild("##firstchild", ImVec2(0, 0));
					{
						ImGui::Checkbox("Box", &Cfg.Box);
						if (Cfg.Box) {
							ImGui::Combo("Style", &Cfg.box_style, "None\0Default\0Edge\0\0", -1);
						}
						ImGui::Checkbox("Name", &Cfg.Name);
						ImGui::Checkbox("Weapons", &Cfg.Weapon);
						ImGui::Checkbox("Health", &Cfg.Health);
						ImGui::Checkbox("Skeleton", &Cfg.skeleton);
						ImGui::Checkbox("Recoil", &Cfg.Recoil);
						if (Cfg.Recoil) {
							ImGui::SliderInt("Size", &Cfg.Size, 1, 15);
						}
						ImGui::Checkbox("Radar", &Cfg.Radar);
						ImGui::Checkbox("Rainbow box", &Cfg.Rainbow);
					}
					ImGui::EndChild();

					ImGui::NextColumn();

					ImGui::BeginChild("##secondchild", ImVec2(0, 0));
					{
						ImGui::Checkbox("No smoke", &Cfg.nosmoke);
						ImGui::Checkbox("No flash", &Cfg.noflash);
						//ImGui::Checkbox("No recoil", &Cfg.novisualrecoil);
						ImGui::Checkbox("No scope border", &Cfg.RemoveScope);
						ImGui::Checkbox("No fog", &Cfg.remove_fog);
						ImGui::Checkbox("Fullbright", &Cfg.fullbright);
						//	ImGui::Checkbox("Hitmarker", &Cfg.hitmarker);
						ImGui::Checkbox("AA Line", &Cfg.anglelines);
						ImGui::Checkbox("Show ranks", &Cfg.showranks);
						ImGui::Checkbox("Foot trils", &Cfg.foottrils);
						/*if (Cfg.foottrils)
						{
							ImGui::SliderFloat("Trails x shift", &Cfg.foottrails_xshift, 0.1, 40);
							ImGui::SliderFloat("Trails y shift", &Cfg.foottrails_yshift, 0.1, 40);
							ImGui::SliderFloat("Trails z shift", &Cfg.foottrails_zshift, 0.1, 40);
						}*/
					}
					ImGui::EndChild();
				}

				else if (active_visuals_tab == 1)
				{
					ImGui::SliderInt("##Vievmodel FOV", &Cfg.viewmodel_fov, 68, 120, "Vievmodel FOV : %.0f");

					ImGui::Checkbox("Ambient light", &Cfg.ambient_light);
					if (Cfg.ambient_light)
					{
						ImGui::SliderFloat("##Ambient light red", &Cfg.mat_ambient_light_r, 0.f, 1.0f, "Ambientlight r : %.2f");
						ImGui::SliderFloat("##Ambient light green", &Cfg.mat_ambient_light_g, 0.f, 1.0f, "Ambientlight g : %.2f");
						ImGui::SliderFloat("##Ambient light blue", &Cfg.mat_ambient_light_b, 0.f, 1.0f, "Ambientlight b : %.2f");
					}

					ImGui::Checkbox("Custom viewmodel FOV", &Cfg.customfov);
					if (Cfg.customfov)
					{
						ImGui::SliderInt("##Custom x FOV", &Cfg.xfov, -25.f, 25.f, "Custom X FOV : %.0f");
						ImGui::SliderInt("##Custom y FOV", &Cfg.yfov, -25.f, 25.f, "Custom Y FOV : %.0f");
						ImGui::SliderInt("##Custom z FOV", &Cfg.zfov, -25.f, 25.f, "Custom Z FOV : %.0f");
					}
					ImGui::Checkbox("Aspect ratio", &Cfg.aspect_ratio);
					if (Cfg.aspect_ratio)
					{
						ImGui::SliderInt("##aspect_ration", &Cfg.aspect_ratio_value, 0.f, 200.f, "Aspect ratio : %.0f");
					}
					ImGui::Checkbox("Thirdperson", &Cfg.thirdperson);
					if (Cfg.thirdperson)
					{
						ImGui::SliderInt("##Distance3rd", &Cfg.thirdperson_dist, 0, 300, "Distance : %.0f");
						ImGui::Hotkey("Third Person Key##3rdp", &Cfg.thirdperson_bind);
					}
					ImGui::Checkbox("Enable agent changer", &Cfg.misc_model);
					if (Cfg.misc_model) {
						ImGui::Combo("Type TT", &Cfg.misc_model_type_tt, agent_types, IM_ARRAYSIZE(agent_types));
						ImGui::Combo("Type CT", &Cfg.misc_model_type_ct, agent_types, IM_ARRAYSIZE(agent_types));
					}
				}

				else if (active_visuals_tab == 2)
				{
					ImGui::Columns(3, nullptr, false);
					ImGui::BeginChild("##firstchild", ImVec2(0, 0));
					{
						ImGui::Checkbox("Player chams##chamsenabled", &Cfg.chams_enable);
						if (Cfg.chams_enable)
						{
							ImGui::SameLine();
							ImGui::ColorEdit3("##chamscolor", Cfg.chams_color, ImGuiColorEditFlags_NoInputs);
							ImGui::Combo("Material##chams", &Cfg.chams_material, ChamsMaterials, ARRAYSIZE(ChamsMaterials));
							ImGui::Checkbox("Wireframe mode##chams", &Cfg.wireframe_mode);
							ImGui::SliderFloat("Alpha##chamsalpha", &Cfg.chams_alpha, 0.0f, 1.0f);
							ImGui::Checkbox("Ignore-Z##chamsingorezenabled", &Cfg.chams_ignorez);
							ImGui::SameLine();
							ImGui::ColorEdit3("##chamscolorignorez", Cfg.chams_color_ignorez, ImGuiColorEditFlags_NoInputs);
							if (Cfg.rage_backtrack || Cfg.RageEnabled)
							{
								ImGui::Text("Backtrack");
								ImGui::SameLine();
								ImGui::ColorEdit3("##chamscolorbacktrack", Cfg.backtrack_color, ImGuiColorEditFlags_NoInputs);
								ImGui::Combo("Backtrack", &Cfg.chams_backtrack, ChamsBacktrack, ARRAYSIZE(ChamsBacktrack));
								ImGui::Combo("Material##backtrack", &Cfg.backtrack_material, ChamsMaterials, ARRAYSIZE(ChamsMaterials));
								ImGui::SliderFloat("Alpha##backtrackchamsalpha", &Cfg.backtrack_alpha, 0.0f, 1.0f);
							}
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##secondchild", ImVec2(0, 0));
					{
						ImGui::Checkbox("Arm chams##chamsenabled", &Cfg.chams_arms_enable);
						if (Cfg.chams_arms_enable)
						{
							ImGui::SameLine();
							ImGui::ColorEdit3("##armscolor", Cfg.chams_arms_color, ImGuiColorEditFlags_NoInputs);
							ImGui::Combo("Material##armchams", &Cfg.chams_arms_material, ChamsMaterials, ARRAYSIZE(ChamsMaterials));
							ImGui::SliderFloat("Alpha##chamsalpha", &Cfg.chams_arms_alpha, 0.0f, 1.0f);
						}

					}
					ImGui::EndChild();
					ImGui::NextColumn();
				}
			}

			if (MenuTab == 4) {

			}

			if (MenuTab == 5) {
				static int active_misc_tab = 0;

				if (Tab("Misc", "", { 100,50 }, active_misc_tab == 0 ? true : false))
					active_misc_tab = 0;

				ImGui::SameLine();

				if (Tab("Config", "", { 100,50 }, active_misc_tab == 1 ? true : false))
					active_misc_tab = 1;

				if (active_misc_tab == 0) {
					ImGui::Columns(3, nullptr, false);
					ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
						ImGui::Checkbox("Bunnyhop", &Cfg.AutoBunnyhop);
						ImGui::Combo("Strafe type", &Cfg.Autostrafe, strafetype, IM_ARRAYSIZE(strafetype));
						if (Cfg.Autostrafe == 1) {
							ImGui::SliderFloat("##Retrack Speed", &Cfg.retrack_speed, 2.0f, 15.0f, "Retrack Speed : %.1f");
						}

						ImGui::Checkbox("Infinite duck", &Cfg.InfiniteDuck);
						if (Cfg.InfiniteDuck) {
							ImGui::Checkbox("Fake duck", &Cfg.FakeDuck);
							if (Cfg.FakeDuck) {
								ImGui::Hotkey("Key##fduck", &Cfg.FakeDuck_Bind);
							}
						}
						ImGui::Checkbox("Slow walk", &Cfg.SlowWalk);
						if (Cfg.SlowWalk) {
							ImGui::SliderInt("##Slow walk", &Cfg.slowwalk_speed, 0, 100, "Slow walk : %.0f");
							ImGui::Hotkey("Key##swalk", &Cfg.SlowWalk_Bind);
						}
						ImGui::SliderInt("##Fakelag", &Cfg.FakelagChoke, 0, 15, "Fakelag : %.0f");
						ImGui::Checkbox("Legit desync", &Cfg.legit_desync);
						if (Cfg.legit_desync) {
							ImGui::Combo("Legit type", &Cfg.legit_aa_type, LegitType, IM_ARRAYSIZE(LegitType));
						}
						ImGui::Checkbox("Clantag", &Cfg.clantag);
						ImGui::Checkbox("Chatspam", &Cfg.chatspam);
						ImGui::Checkbox("Damage log", &Cfg.damage_show);
						ImGui::Checkbox("Hitsound", &Cfg.hitsound);
						ImGui::Checkbox("Hit effect", &Cfg.hiteffect);
						ImGui::Checkbox("Kill Counter", &Cfg.kill_counter);
						ImGui::Checkbox("Nightmode", &Cfg.misc_nightmode);
						if (Cfg.misc_nightmode)
							ImGui::SliderFloat("##misc_nightmode", &Cfg.misc_nightmode_value, 0.03f, 1.00f, "Nightmode : %.2f");

						//ImGui::Combo("Skybox", &Cfg.skybox, skybox_type, ARRAYSIZE(skybox_type));
						ImGui::Checkbox("Watermark", &Cfg.watermark);
						if (ImGui::Button("Unload")) {
							uninject = true;
						}
					}
					ImGui::EndChild();

					ImGui::NextColumn();

					ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {

						ImGui::Checkbox("Enable buybot", &Cfg.buybot);
						if (Cfg.buybot) {
							ImGui::Combo("Pistols", &Cfg.buybot_pistol, Pistols, IM_ARRAYSIZE(Pistols));
							ImGui::Combo("Weapon", &Cfg.buybot_weapon, Weapons, IM_ARRAYSIZE(Weapons));


							std::vector<std::string> buyitem =
							{
								"Molotov",
								"Grenade",
								"Smoke",
								"Flash",
								"Decoy",
								"Armor",
								"Zeus",
								"Defuser"
							};

							static std::string item = "";

							if (ImGui::BeginCombo("Other", item.c_str())) {
								std::string preview = "";
								if (Cfg.buybot_grenade[0]) {
									preview += "inc, ";
								}
								if (Cfg.buybot_grenade[1]) {
									preview += "he, ";
								}
								if (Cfg.buybot_grenade[2]) {
									preview += "smoke, ";
								}
								if (Cfg.buybot_grenade[3]) {
									preview += "flash, ";
								}
								if (Cfg.buybot_grenade[4]) {
									preview += "decoy, ";
								}
								if (Cfg.buybot_grenade[5]) {
									preview += "hk, ";
								}
								if (Cfg.buybot_grenade[6]) {
									preview += "zeus; ";
								}
								if (Cfg.buybot_grenade[7]) {
									preview += "kit, ";
								}
								item = preview;

								for (int i = 0; i < 8; i++) {
									ImGui::Selectable(buyitem[i].c_str(), &Cfg.buybot_grenade[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								}
								ImGui::EndCombo();
							}

						}

					}
					ImGui::EndChild();
					ImGui::NextColumn();

					ImGui::BeginChild("##thirdchild", ImVec2(0, 0)); {
						ImGui::Checkbox("No shadows", &Cfg.noshadows);
						ImGui::Checkbox("Disable panorama blur", &Cfg.disablepanoramablur);
						ImGui::Checkbox("No 3dsky", &Cfg.no3dsky);
						ImGui::Checkbox("Bloom enable", &Cfg.bloom);
						if (Cfg.bloom) {
							ImGui::SliderInt("Value##bloom", &Cfg.bloom_value, 0, 100, "Value bloom: %.0f");
							ImGui::SliderInt("Model ambient##bloom", &Cfg.model_ambient_value, 0, 100, "Model ambient: %.0f");
						}
						ImGui::Checkbox("Render distance", &Cfg.r_farz);
						if (Cfg.r_farz) {
							ImGui::SliderFloat("Value##boost", &Cfg.r_farz_value, 0.f, 8000.f, "Value: %.0f");
						}
						ImGui::Checkbox("No postprocessing", &Cfg.noprocessing);
					}
					ImGui::EndChild();
				}

				else if (active_misc_tab == 1) {
					ImGui::Columns(2, nullptr, false);
					ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
						ImGui::Text("Cfg");
						static int a1 = 0;
						ImGui::Combo("##select", &a1, items, 6);

						if (ImGui::ButtonEx(XorStr("Load"), ImVec2(60, 24))) {
							CConfig::Get().Load(items[a1]);
						}
						ImGui::SameLine();
						if (ImGui::ButtonEx(XorStr("Save"), ImVec2(60, 24))) {
							CConfig::Get().Save(items[a1]);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {
						ImGui::Text("Lua(who)");
					}
					ImGui::EndChild();

				}

			}

		}
		ImGui::EndChild();
	}
	ImGui::End();

	ImGui::PopFont();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::Show()
{
	Visible = true;
}

void Menu::Hide()
{
	Visible = false;
}

void Menu::Toggle()
{
	Visible = !Visible;
}

void Menu::CreateStyle()
{
	ImVec4 col_text = ImColor(150, 150, 150, 255);
	ImVec4 col_main = ImVec4(0.09f, .09f, .09f, 1.f);
	ImVec4 col_back = ImColor(31, 44, 54);
	ImVec4 col_area = ImColor(4, 32, 41);

	ImGui::GetIO().Fonts->AddFontDefault();

	ImGuiStyle* style = &ImGui::GetStyle();
	
	style->FramePadding = ImVec2(4, 2);
	style->ItemSpacing = ImVec2(6, 3);
	style->ItemInnerSpacing = ImVec2(6, 4);
	style->Alpha = 1.f;
	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;
	style->IndentSpacing = 6.0f;
	style->ItemInnerSpacing = ImVec2(3, 4);
	style->ColumnsMinSpacing = 50.0f;
	style->GrabMinSize = 14.0f;
	style->GrabRounding = 16.0f;
	style->ScrollbarSize = 12.0f;
	style->ScrollbarRounding = 16.0f;
	style->AntiAliasedLines = true;
	
	Style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_Text2] = ImColor(255, 55, 55, 255);
	Style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_WindowBg] = ImColor(9, 9, 9, 255);
	Style.Colors[ImGuiCol_ChildWindowBg] = ImColor(10, 10, 10, 255);
	Style.Colors[ImGuiCol_PopupBg] = ImColor(4, 4, 4, 255);
	Style.Colors[ImGuiCol_Border] = ImColor(25, 25, 25, 255);
	Style.Colors[ImGuiCol_BorderShadow] = ImColor(25, 25, 25, 255);
	Style.Colors[ImGuiCol_FrameBg] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_FrameBgActive] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBg] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBgActive] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_MenuBarBg] = ImColor(255, 55, 55, 255);
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
	Style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(87, 48, 161, 255);
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(87, 48, 161, 255);
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(73, 6, 130, 255);
	Style.Colors[ImGuiCol_CheckMark] = ImColor(87, 48, 161, 255);  //87, 48, 161


	Style.Colors[ImGuiCol_SliderGrab] = ImColor(87, 48, 161, 255);
	Style.Colors[ImGuiCol_SliderGrabActive] = ImColor(73, 6, 130, 255);  //73, 6, 130

	Style.Colors[ImGuiCol_Button] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_ButtonHovered] = ImColor(57, 60, 67, 255);
	Style.Colors[ImGuiCol_ButtonActive] = ImColor(57, 60, 67, 255);
	Style.Colors[ImGuiCol_Header] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_HeaderHovered] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_HeaderActive] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	Style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	Style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	ImGui::GetStyle() = Style;
}

