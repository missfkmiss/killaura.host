#include "options.hpp"
#include "valve_sdk/misc/Enums.hpp"

Options Cfg;
std::string ActiveWeaponName = "Knife/Other";
bool   uninject = false;

namespace Globals
{
	bool WeaponTabValid = false;
}