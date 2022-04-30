#pragma once
#pragma once

#include <string>
#include "../singleton.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/directx9/imgui_impl_dx9.h"
#include "../valve_sdk/structs.hpp"
#include "../helpers/input.hpp"
#include "../options.hpp"
#include <d3dx9tex.h>

#pragma comment(lib,"d3dx9.lib")

namespace ImGui {
	bool Hotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0, 0));
}

struct IDirect3DDevice9;

class Menu
	: public Singleton<Menu>
{
public:
	void Initialize();
	void Shutdown();
	void OnDeviceLost();
	void OnDeviceReset();
	void CreateFont();

	void RenderMenu();

	ImFont* font_icon;
	ImFont* font_icon2;
	ImFont* font;
	ImFont* espfont;

	void Show();
	void Hide();
	void Toggle();

	void apply_fonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		//ImFont* font_icon = io.Fonts->AddFontFromFileTTF("Icon_Font.ttf", 50);
		//ImFont* espfont = io.Fonts->AddFontFromFileTTF("smallest_pixel-7.ttff", 14);

		//font_icon = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Icon_Font.ttf", 50.0f);

		font_icon2 = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Astrix.ttf", 35.0f);
	}

	bool IsVisible() const { return Visible; }

private:
	void CreateStyle();

	ImGuiStyle	Style;
	bool		Visible;
};
