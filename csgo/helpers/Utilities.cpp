#include "Utilities.hpp"

#define NOMINMAX
#include <algorithm>
#include <Windows.h>
#include <cstdio>
#include <string>
#include <vector>
#include <Psapi.h>

#include "../options.hpp"
#include "../valve_sdk/structs.hpp"
#include "math.hpp"
#ifndef max
#define max(a ,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace Utilities
{
	std::vector<char> HexToBytes(const std::string& hex) {
		std::vector<char> res;

		for (auto i = 0u; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			res.push_back(byte);
		}

		return res;
	}
	std::string BytesToString(unsigned char* data, int len) {
		constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
									'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		std::string res(len * 2, ' ');
		for (int i = 0; i < len; ++i) {
			res[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
			res[2 * i + 1] = hexmap[data[i] & 0x0F];
		}
		return res;
	}

	std::vector<std::string> Split(const std::string& str, const char* delim) {
		std::vector<std::string> res;
		char* pTempStr = _strdup(str.c_str());
		char* context = NULL;
		char* pWord = strtok_s(pTempStr, delim, &context);
		while (pWord != NULL) {
			res.push_back(pWord);
			pWord = strtok_s(NULL, delim, &context);
		}

		free(pTempStr);

		return res;
	}

	unsigned int FindInDataMap(datamap_t* pMap, const char* name) {
		while (pMap) {
			for (int i = 0; i < pMap->dataNumFields; i++) {
				if (pMap->dataDesc[i].fieldName == NULL) continue;
				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0) return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];
				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED) {
					if (pMap->dataDesc[i].td) {
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0) return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}

	void AttachConsole() {
		_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_old_err = GetStdHandle(STD_ERROR_HANDLE);
		_old_in = GetStdHandle(STD_INPUT_HANDLE);

		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

		_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_err = GetStdHandle(STD_ERROR_HANDLE);
		_in = GetStdHandle(STD_INPUT_HANDLE);

		SetConsoleMode(_out, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
		SetConsoleMode(_in, ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
	}

	void DetachConsole() {
		if (_out && _err && _in) {
			FreeConsole();

			if (_old_out) SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
			if (_old_err) SetStdHandle(STD_ERROR_HANDLE, _old_err);
			if (_old_in) SetStdHandle(STD_INPUT_HANDLE, _old_in);
		}
	}

	bool ConsolePrint(const char* fmt, ...) {
		if (!_out) return false;

		char buf[1024];
		va_list va;

		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);

		return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
	}

	char ConsoleReadKey() {
		if (!_in) return false;

		auto key = char{ 0 };
		auto keysread = DWORD{ 0 };

		ReadConsoleA(_in, &key, 1, &keysread, nullptr);

		return key;
	}
#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
	uintptr_t FindSignature(const char* szModule, const char* szSignature)
	{
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		DWORD rangeStart = reinterpret_cast<DWORD>(GetModuleHandleA(szModule));
		MODULEINFO miModInfo;
		GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(rangeStart), &miModInfo, sizeof(MODULEINFO));
		DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
		for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
		{
			if (!*pat)
				return firstMatch;

			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GET_BYTE(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;

				if (!pat[2])
					return firstMatch;

				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;

				else
					pat += 2;
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}
	int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules) {
		bool signaled[32] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if (timeout == 0) {
			for (auto& mod : modules) {
				if (GetModuleHandleW(std::data(mod)) == NULL) return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if (timeout < 0) timeout = INT32_MAX;

		while (true) {
			for (auto i = 0u; i < modules.size(); ++i) {
				auto& module = *(modules.begin() + i);
				if (!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
					signaled[i] = true;

					bool done = true;
					for (auto j = 0u; j < modules.size(); ++j) {
						if (!signaled[j]) {
							done = false;
							break;
						}
					}
					if (done) {
						success = true;
						goto exit;
					}
				}
			}
			if (totalSlept > std::uint32_t(timeout)) break;
			Sleep(10);
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}
	std::uint8_t* PatternScan(void* module, const char* signature) {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?') ++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) return &scanBytes[i];
		}
		return nullptr;
	}

	DWORD WaitOnModuleHandle(std::string moduleName) {
		DWORD ModuleHandle = NULL;
		while (!ModuleHandle) {
			ModuleHandle = (DWORD)GetModuleHandleA(moduleName.c_str());
			if (!ModuleHandle)
				Sleep(50);
		}
		return ModuleHandle;
	}

	bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask) {
		for (; *szMask; ++szMask, ++Mask, ++Data) {
			if (*szMask == 'x' && *Mask != *Data) {
				return false;
			}
		}
		return (*szMask) == 0;
	}

	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask) {
		DWORD Address = WaitOnModuleHandle(moduleName.c_str());
		MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
		DWORD Length = ModInfo.SizeOfImage;
		for (DWORD c = 0; c < Length; c += 1) {
			if (bCompare((BYTE*)(Address + c), Mask, szMask)) {
				return DWORD(Address + c);
			}
		}
		return 0;
	}

	void SetClantag(const char* tag) {
		static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

		fnClantagChanged(tag, tag);
	}

	void SetName(const char* name) {
		static auto nameConvar = g_CVar->FindVar("name");
		nameConvar->m_fnChangeCallbacks.m_Size = 0;

		static auto do_once = (nameConvar->SetValue("\n���"), true);

		nameConvar->SetValue(name);
	}

	void RankRevealAll() {
		g_CHLClient->DispatchUserMessage(50, 0, 0, nullptr);
	}

	float GetLerpTime() {
		int ud_rate = g_CVar->FindVar("cl_updaterate")->GetInt();
		ConVar* min_ud_rate = g_CVar->FindVar("sv_minupdaterate");
		ConVar* max_ud_rate = g_CVar->FindVar("sv_maxupdaterate");

		if (min_ud_rate && max_ud_rate) ud_rate = max_ud_rate->GetInt();

		float ratio = g_CVar->FindVar("cl_interp_ratio")->GetFloat();

		if (ratio == 0) ratio = 1.0f;

		float lerp = g_CVar->FindVar("cl_interp")->GetFloat();
		ConVar* c_min_ratio = g_CVar->FindVar("sv_client_min_interp_ratio");
		ConVar* c_max_ratio = g_CVar->FindVar("sv_client_max_interp_ratio");

		if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1) ratio = std::clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

		return max(lerp, (ratio / ud_rate));
	}

	bool IsTickValid(float SimulationTime, float MaxTime) {
		INetChannelInfo* NetChannelInfo = g_EngineClient->GetNetChannelInfo();
		if (!NetChannelInfo) return true;
		float Correct = 0;
		Correct += NetChannelInfo->GetLatency(FLOW_OUTGOING);
		Correct += NetChannelInfo->GetLatency(FLOW_INCOMING);
		Correct += GetLerpTime();

		std::clamp(Correct, 0.f, g_CVar->FindVar("sv_maxunlag")->GetFloat());

		float DeltaTime = Correct - (g_GlobalVars->curtime - SimulationTime);

		float TimeLimit = MaxTime;
		std::clamp(TimeLimit, 0.001f, 0.2f);

		if (fabsf(DeltaTime) > TimeLimit) return false;

		return true;
	}

	bool IsBattleRoyale() {
		static ConVar* game_type = nullptr;
		if (!game_type) game_type = g_CVar->FindVar("game_type");
		return (game_type->GetInt() == 6);
	}

	void autoBhop(CUserCmd* cmd) {
		if (Cfg.AutoBunnyhop) {
			static bool jumped_last_tick = false;
			static bool should_fake_jump = false;

			if (!jumped_last_tick && should_fake_jump) {
				should_fake_jump = false;
				cmd->buttons |= IN_JUMP;
			}
			else if (cmd->buttons & IN_JUMP) {
				if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
					jumped_last_tick = true;
					should_fake_jump = true;
				}
				else if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER) {
					jumped_last_tick = true;
					should_fake_jump = true;
				}
				else {
					cmd->buttons &= ~IN_JUMP;
					jumped_last_tick = false;
				}
			}
			else {
				jumped_last_tick = false;
				should_fake_jump = false;
			}
		}
	}

	int pWeaponItemIndexData[37] = {
		WEAPON_DEAGLE,WEAPON_ELITE,WEAPON_FIVESEVEN,
		WEAPON_GLOCK,WEAPON_HKP2000,WEAPON_P250,
		WEAPON_USP_SILENCER,WEAPON_CZ75A,WEAPON_REVOLVER,
		WEAPON_TEC9,

		WEAPON_AK47,WEAPON_AUG,WEAPON_FAMAS,WEAPON_GALILAR,
		WEAPON_M249,WEAPON_M4A1,WEAPON_M4A1_SILENCER,WEAPON_MAC10,
		WEAPON_P90,WEAPON_MP5SD,WEAPON_UMP45,WEAPON_XM1014,WEAPON_BIZON,
		WEAPON_MAG7,WEAPON_NEGEV,WEAPON_SAWEDOFF,
		WEAPON_MP7,WEAPON_MP9,WEAPON_NOVA,WEAPON_SG556,
		WEAPON_SCAR20,WEAPON_G3SG1,

		WEAPON_AWP,WEAPON_SSG08,
		WEAPON_KNIFE,WEAPON_KNIFE_T,WEAPON_TASER
	};

	std::string pWeaponData[37] = {
		"Desert Eagle","Dual Berettas","Five-SeveN",
		"Glock-18","P2000","P250",
		"USP-S","CZ75 Auto","R8 Revolver",
		"Tec-9",

		"AK-47","AUG","FAMAS","Galil AR",
		"M249","M4A4","M4A1-S","MAC-10"	,
		"P90","MP5-SD","UMP-45","XM1014","PP-Bizon",
		"MAG-7","Negev","Sawed-Off",
		"MP7","MP9","Nova","SG 553",
		"SCAR-20","G3SG1",

		"AWP","SSG 08",
		"Knife","Knife","ZeusX27"
	};

	int GetWeaponSettingsSelectID() {
		for (size_t i = 0; i < (sizeof(pWeaponData) / sizeof(*pWeaponData)); i++) {
			if (pWeaponItemIndexData[i] == g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex())
				return i;
		}

		return -1;
	}

	bool IsInGame() {
		return g_EngineClient->IsConnected() && g_EngineClient->IsInGame();
	}

	float GetFovToPlayer(QAngle viewAngle, QAngle aimAngle) {
		QAngle delta = aimAngle - viewAngle;
		Math::NormalizeAngles(delta);
		Math::ClampAngles(delta);
		return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
	}

	bool HitChance(float hitchance) {
		if (g_LocalPlayer->m_hActiveWeapon())
			return false;

		if (hitchance > 0 && g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER) {
			float Inaccuracy = g_LocalPlayer->m_hActiveWeapon()->GetInaccuracy();

			if (Inaccuracy == 0) {
				Inaccuracy = 0.0000001;
			}

			Inaccuracy = 1 / Inaccuracy;
			return (((hitchance * 1.5f) <= Inaccuracy) ? true : false);
		}

		return true;
	}

	bool LineGoesThroughSmoke(Vector vStartPos, Vector vEndPos) {
		static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))Utilities::PatternScan(GetModuleHandleA(("client.dll")), ("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
		return LineGoesThroughSmokeFn(vStartPos, vEndPos);
	}

	void AutoStrafe(CUserCmd* cmd, QAngle va) {
		switch (Cfg.Autostrafe) {
		case 1: {
			if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
				return;

			bool on_ground = (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && !(cmd->buttons & IN_JUMP);
			if (on_ground) {
				return;
			}

			static auto side = 1.0f;
			side = -side;

			auto velocity = g_LocalPlayer->m_vecVelocity();
			velocity.z = 0.0f;

			QAngle wish_angle = cmd->viewangles;

			auto speed = velocity.Length2D();
			auto ideal_strafe = std::clamp(RAD2DEG(atan(15.f / speed)), 0.0f, 90.0f);

			if (cmd->forwardmove > 0.0f)
				cmd->forwardmove = 0.0f;

			static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");

			static float old_yaw = 0.f;
			auto yaw_delta = std::remainderf(wish_angle.yaw - old_yaw, 360.0f);
			auto abs_angle_delta = abs(yaw_delta);
			old_yaw = wish_angle.yaw;

			if (abs_angle_delta <= ideal_strafe || abs_angle_delta >= 30.0f) {
				QAngle velocity_direction;
				Math::VectorAngles(velocity, velocity_direction);
				auto velocity_delta = std::remainderf(wish_angle.yaw - velocity_direction.yaw, 360.0f);
				auto retrack = std::clamp(RAD2DEG(atan(30.0f / speed)), 0.0f, 90.0f) * Cfg.retrack_speed;
				if (velocity_delta <= retrack || speed <= 15.0f) {
					if (-(retrack) <= velocity_delta || speed <= 15.0f) {
						wish_angle.yaw += side * ideal_strafe;
						cmd->sidemove = cl_sidespeed->GetFloat() * side;
					}
					else {
						wish_angle.yaw = velocity_direction.yaw - retrack;
						cmd->sidemove = cl_sidespeed->GetFloat();
					}
				}
				else {
					wish_angle.yaw = velocity_direction.yaw + retrack;
					cmd->sidemove = -cl_sidespeed->GetFloat();
				}

				Math::MovementFix(cmd, wish_angle, cmd->viewangles);
			}
			else if (yaw_delta > 0.0f) {
				cmd->sidemove = -cl_sidespeed->GetFloat();
			}
			else if (yaw_delta < 0.0f) {
				cmd->sidemove = cl_sidespeed->GetFloat();
			}
		}break;

		case 2: {
			static bool is_bhopping;
			static float calculated_direction;
			static bool in_transition;
			static float true_direction;
			static float wish_direction;
			static float step;
			static float rough_direction;

			if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
				return;

			bool on_ground = (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && !(cmd->buttons & IN_JUMP);
			if (on_ground) {
				return;
			}

			enum directions {
				FORWARDS = 0,
				BACKWARDS = 180,
				LEFT = 90,
				RIGHT = -90
			};

			// Reset direction when player is not strafing
			is_bhopping = cmd->buttons & IN_JUMP;
			if (!is_bhopping && g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
				calculated_direction = directions::FORWARDS;
				in_transition = false;
				return;
			}

			// Get true view angles
			QAngle base{ };
			g_EngineClient->GetViewAngles(base);

			// Calculate the rough direction closest to the player's true direction
			auto get_rough_direction = [&](float true_direction) -> float {
				// Make array with our four rough directions
				std::array< float, 4 > minimum = { directions::FORWARDS, directions::BACKWARDS, directions::LEFT, directions::RIGHT };
				float best_angle, best_delta = 181.f;

				// Loop through our rough directions and find which one is closest to our true direction
				for (size_t i = 0; i < minimum.size(); ++i) {
					float rough_direction = base.yaw + minimum.at(i);
					float delta = fabsf(Math::NormalizeYaw(true_direction - rough_direction));

					// Only the smallest delta wins out
					if (delta < best_delta) {
						best_angle = rough_direction;
						best_delta = delta;
					}
				}

				return best_angle;
			};

			// Get true direction based on player velocity
			true_direction = g_LocalPlayer->m_vecVelocity().Angle().y;

			// Detect wish direction based on movement keypresses
			if (cmd->buttons & IN_FORWARD) {
				wish_direction = base.yaw + directions::FORWARDS;
			}
			else if (cmd->buttons & IN_BACK) {
				wish_direction = base.yaw + directions::BACKWARDS;
			}
			else if (cmd->buttons & IN_MOVELEFT) {
				wish_direction = base.yaw + directions::LEFT;
			}
			else if (cmd->buttons & IN_MOVERIGHT) {
				wish_direction = base.yaw + directions::RIGHT;
			}
			else {
				// Reset to forward when not pressing a movement key, then fix anti-aim strafing by setting IN_FORWARD
				cmd->buttons |= IN_FORWARD;
				wish_direction = base.yaw + directions::FORWARDS;
			}
			// Calculate the ideal strafe rotation based on player speed (c) navewindre
			float speed_rotation = min(RAD2DEG(std::asinf(30.f / g_LocalPlayer->m_vecVelocity().Length2D())) * 0.5f, 45.f);
			if (in_transition) {
				// Get value to rotate by via calculated speed rotation
				float ideal_step = speed_rotation + calculated_direction;
				step = fabsf(Math::NormalizeYaw(calculated_direction - ideal_step)); // 15.f is a good alternative, but here is where you would do your "speed" slider value for the autostrafer

				// Check when the calculated direction arrives close to the wish direction
				if (fabsf(Math::NormalizeYaw(wish_direction - calculated_direction)) > step) {
					float add = Math::NormalizeYaw(calculated_direction + step);
					float sub = Math::NormalizeYaw(calculated_direction - step);

					// Step in direction that gets us closer to our wish direction
					if (fabsf(Math::NormalizeYaw(wish_direction - add)) >= fabsf(Math::NormalizeYaw(wish_direction - sub))) {
						calculated_direction -= step;
					}
					else {
						calculated_direction += step;
					}
				}
				else
				{
					// Stop transitioning when we meet our wish direction
					in_transition = false;
				}
			}
			else {
				// Get rough direction and setup calculated direction only when not transitioning
				rough_direction = get_rough_direction(true_direction);
				calculated_direction = rough_direction;

				// When we have a difference between our current (rough) direction and our wish direction, then transition
				if (rough_direction != wish_direction) {
					in_transition = true;
				}
			}

			// Set movement up to be rotated
			cmd->forwardmove = 0.f;
			cmd->sidemove = cmd->command_number % 2 ? 450.f : -450.f;

			// Calculate ideal rotation based on our newly calculated direction
			float direction = (cmd->command_number % 2 ? speed_rotation : -speed_rotation) + calculated_direction;

			// Rotate our direction based on our new, defininite direction
			float rotation = DEG2RAD(base.yaw - direction);

			float cos_rot = cos(rotation);
			float sin_rot = sin(rotation);

			float forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
			float sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

			// Apply newly rotated movement
			cmd->forwardmove = forwardmove;
			cmd->sidemove = sidemove;
		}break;
		}
	}
}