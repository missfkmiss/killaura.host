#include "structs.hpp"
#include "../Helpers/math.hpp"
#include "../Helpers/Utilities.hpp"

CGameRules* g_GameRules = nullptr;
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
bool C_BaseEntity::IsPlayer()
{
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 158)(this);
}

bool C_BaseEntity::IsWeapon()
{
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 166)(this);
}

bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

/*
CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData() {
	
	//static auto fnGetWpnData
	//	= reinterpret_cast<CCSWeaponInfo * (__thiscall*)(void*)>(
	//		Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B ? ? ? ? 85 C9 75 04 33 FF EB 2F")
	//		);
	//return fnGetWpnData(this);

	//return CallVFunction<CCSWeaponInfo * (__thiscall*)(void*)>(this, 457)(this);
}*/

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	static auto fnGetWpnData
		= reinterpret_cast<CCSWeaponInfo * (__thiscall*)(void*)>(
			Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B ? ? ? ? 85 C9 75 04 33 FF EB 2F")
			);
	return fnGetWpnData(this);
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	static decltype(this) stored_weapon = nullptr;
	static int stored_tick = 0;

	if (stored_weapon != this || stored_tick >= g_LocalPlayer->m_nTickBase()) 
	{
		stored_weapon = this;
		stored_tick = g_LocalPlayer->m_nTickBase();
		return false;
	}

	if (IsReloading() || m_iClip1() <= 0 || !g_LocalPlayer) return false;

	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsZeus()
{
	return this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER;
}

//bool C_BaseCombatWeapon::IsGun()
//{
//	switch (GetCSWeaponData()->iWeaponType)
//	{
//	case WEAPONTYPE_C4:                оно
//		return false;                
//	case WEAPONTYPE_GRENADE:            к
//		return false;                   р
//	case WEAPONTYPE_KNIFE:              а
//		return false;                   ш
//	case WEAPONTYPE_UNKNOWN:            и
//		return false;			        т
//	default:
//		return true;
//	}
//}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

/*
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
		*/

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

//--------------------------------------------------------------------

bool C_BaseCombatWeapon::PistolLite()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_USP_SILENCER:
		return true;
	case WEAPON_HKP2000:
		return true;
	case WEAPON_GLOCK:
		return true;
	case WEAPON_TEC9:
		return true;
	case WEAPON_CZ75A:
		return true;
	case WEAPON_FIVESEVEN:
		return true;
	case WEAPON_P250:
		return true;
	case WEAPON_ELITE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::H_Pistol()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_REVOLVER:
		return true;
	case WEAPON_DEAGLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::Scout()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_SSG08:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::AutoScar()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_SCAR20:
		return true;
	case WEAPON_G3SG1:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::AWP()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_AWP:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::Rifle()
{
	switch (m_Item().m_iItemDefinitionIndex())
	{
	case WEAPON_AK47:
		return true;
	case WEAPON_AUG:
		return true;
	case WEAPON_FAMAS:
		return true;
	case WEAPON_GALILAR:
		return true;
	case WEAPON_M4A1:
		return true;
	case WEAPON_SG556:
		return true;
	case WEAPON_M4A1_SILENCER:
		return true;
	default:
		return false;
	}
}
//доделать
//--------------------------------------------------------------------
bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static uint32_t inReload = *(uint32_t*)(Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 483)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 452)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 484)(this);
}

//CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
//{	static unsigned int inReload = *(uint32_t*)(Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "83 BE ? ? ? ? ? 7F 67") + 2) - 12;
//	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
//}

//bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
//{
//	static uint32_t currentCommand = *(uint32_t*)(Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "C6 86 ? ? ? ? ? FF 50 04") + 2);
//	return (bool*)((uintptr_t)this + currentCommand);
//}

bool C_BasePlayer::SetupBonesS(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	auto backupval = *reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274);

	*reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274) = 0;
	bool setuped_bones = this->SetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	*reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274) = backupval;

	return setuped_bones;
}

//bool C_BasePlayer::IsNotTarget()
//{
//	return !this || !IsPlayer() || this == g_LocalPlayer || IsDormant() || !IsAlive() || m_bGunGameImmunity() || (m_fFlags() & FL_FROZEN) || GetClientClass()->m_ClassID != ClassId_CCSPlayer;
//}

bool C_BasePlayer::IsTeammate()
{
	return !Utilities::IsBattleRoyale() && this->m_iTeamNum() == g_LocalPlayer->m_iTeamNum();
}

//CUserCmd*& C_BasePlayer::m_pCurrentCommand()
//{
//	static uint32_t currentCommand = *(uint32_t*)(Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
//	return *(CUserCmd**)((uintptr_t)this + currentCommand);
//}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}
bool& C_BasePlayer::ClientAnimations()
{
	return *reinterpret_cast<bool*>(uintptr_t(this) + NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_bClientSideAnimation"));
}
void C_BasePlayer::Animations(bool active)
{
	if (this->GetPlayerAnimState()->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		this->GetPlayerAnimState()->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount - 1;

	ClientAnimations() = active;
}
AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	return *(AnimationLayer**)((DWORD)this + 0x2990);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15) return &GetAnimOverlays()[i];
	return nullptr;
}
void C_BasePlayer::SetAbsAngles(QAngle angle)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle & Angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)Utilities::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");
	SetAbsAngles(this, angle);
}
int C_BasePlayer::GetSequenceActivity(int sequence)
{
	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr) return -1;

	static int (__fastcall* get_sequence_activity)(void*, studiohdr_t*, int) = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>
		(Utilities::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 7D 08 FF 56 8B F1 74 3D"));
	return get_sequence_activity(this, hdr, sequence);
}
float C_BasePlayer::MaxDesyncDelta() 
{
	auto animstate = g_LocalPlayer->GetPlayerAnimState();

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;

	if (duckammount > 0) {
		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
	}

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return unk3;
}
CBasePlayerAnimState*C_BasePlayer::GetPlayerAnimState()
{
	return *(CBasePlayerAnimState **)((DWORD)this + 0x9960);
}

void C_BasePlayer::UpdateAnimationState(CBasePlayerAnimState*state, QAngle angle)
{
	static uint8_t* UpdateAnimState = Utilities::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
	if (!UpdateAnimState) return;

	__asm { push 0 }

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(CBasePlayerAnimState*state)
{
	using ResetAnimState_t = void(__thiscall*)(CBasePlayerAnimState*);
	static ResetAnimState_t ResetAnimState = (ResetAnimState_t)Utilities::PatternScan(GetModuleHandleA("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState) return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(CBasePlayerAnimState*state)
{
	using CreateAnimState_t = void(__thiscall*)(CBasePlayerAnimState*, C_BasePlayer*);
	static CreateAnimState_t CreateAnimState = (CreateAnimState_t)Utilities::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState) return;

	CreateAnimState(state, this);
}

std::string C_BasePlayer::GetName()
{
	bool console_safe;
	// Cleans player's name so we don't get new line memes. Use this everywhere you get the players name.
	// Also, if you're going to use the console for its command and use the players name, set console_safe.
	player_info_t pinfo = this->GetPlayerInfo();

	char* pl_name = pinfo.szName;
	char buf[128];
	int c = 0;

	for (int i = 0; pl_name[i]; ++i)
	{
		if (c >= sizeof(buf) - 1)
			break;

		switch (pl_name[i])
		{
		case '"': if (console_safe) break;
		case '\\':
		case ';': if (console_safe) break;
		case '\n':
			break;
		default:
			buf[c++] = pl_name[i];
		}
	}

	buf[c] = '\0';
	return std::string(buf);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::IsAlive()
{
	return this->m_iHealth() > 0;
}

bool C_BasePlayer::IsFlashed()
{
	static uint32_t m_flFlashMaxAlpha = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}

bool C_BasePlayer::HasC4()
{
	static bool (__thiscall* fnHasC4)(void*) = reinterpret_cast<bool(__thiscall*)(void*)>(Utilities::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31"));
	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) 
	{
		studiohdr_t* studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) 
		{
			mstudiobbox_t* hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) 
			{
				class Vector
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}
Vector C_BasePlayer::GetHitboxPos(int hitbox_id, matrix3x4_t* boneMatrix)
{
	studiohdr_t* studio_model = g_MdlInfo->GetStudiomodel(GetModel());
	if (studio_model) 
	{
		mstudiobbox_t* hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
		if (hitbox) 
		{
			Vector min = Vector{}, max = Vector{};

			Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			return (min + max) / 2.0f;
		}
	}
	return Vector{};
}

mstudiobbox_t* C_BasePlayer::GetHitbox(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) 
	{
		studiohdr_t* studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) 
		{
			mstudiobbox_t* hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) 
			{
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX) return false;

	const model_t *model = this->GetModel();
	if (!model) return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr) return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0)) return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox) return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) 
	{
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	Vector endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return CallVFunction<void(__thiscall*)(void*)>(this, 224)(this);
}

//void C_BasePlayer::InvalidateBoneCache()
//{
//	static DWORD addr = (DWORD)Utilities::PatternScan(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");
//
//	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount;
//	*(int*)((uintptr_t)this + 0xA28) = 0;
//
//	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
//	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF;
//	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1);
//}

int C_BasePlayer::m_nMoveType()
{
	return *(int*)((uintptr_t)this + 0x25C);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static uint32_t deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}
void C_BaseEntity::SetGloveModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

//void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
//{
//	return CallVFunction<void(__thiscall*)(void*, int)>(this, 246)(this, sequence);
//}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA360);
}

//bool C_BasePlayer::FakeDucking()
//{
//	static float storedTick;
//	static float crouchedTicks[65];
//
//	if (!this->GetPlayerAnimState()) return false;
//
//	float m_flDuckAmount = this->GetPlayerAnimState()->m_fDuckAmount;
//	float m_flDuckSpeed = this->m_flDuckSpeed();
//
//	if (m_flDuckSpeed != 0.f && m_flDuckAmount != 0.f)
//	{
//		if (m_flDuckSpeed == 8.f && m_flDuckAmount <= 0.9f && m_flDuckAmount > 0.01f)
//		{
//			if (storedTick != g_GlobalVars->tickcount)
//			{
//				crouchedTicks[this->EntIndex()] = crouchedTicks[this->EntIndex()] + 1;
//				storedTick = g_GlobalVars->tickcount;
//			}
//			return (crouchedTicks[this->EntIndex()] >= 5);
//		}
//		else crouchedTicks[this->EntIndex()] = 0;
//	}
//
//	return false;
//}

//int C_BasePlayer::GetTickBase()
//{
//	return (int)((DWORD)this + NetvarSys::Get().GetOffset("DT_BasePlayer", "m_nTickBase"));
//}