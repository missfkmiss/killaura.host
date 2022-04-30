#define NOMINMAX

#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/Utilities.hpp"

#include "hooks.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "Features/menu.hpp"
#include "features/visuals.hpp"
#include "config.hpp"

DWORD WINAPI OnDllAttach(LPVOID base)
{
	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(1000);

	Interfaces::Initialize();
	NetvarSys::Get().Initialize();
	InputSys::Get().Initialize();
	Menu::Get().Initialize();
	CConfig::Get().Initialize();
	Hooks::Initialize();

	InputSys::Get().RegisterHotkey(VK_INSERT, [base]() { Menu::Get().Toggle(); });

	while (!uninject)
		Sleep(1000);

	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDll);
		CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
		return TRUE;
	}
}

bool FileExists(std::string& file)
{
	std::ifstream in(file);
	return in.good();
}