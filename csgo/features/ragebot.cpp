#include "ragebot.hpp"
#include "../hooks.hpp"
#include "autowall.hpp"
#include "../Features/visuals.hpp"
#include "../helpers/Utilities.hpp"

float flOldCurtime;
float flOldFrametime;
void FakeLag::FakeDuck(CUserCmd* cmd, bool& bSendPacket) {
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		return;
	if (!g_LocalPlayer->IsAlive())
		return;

	static int cnt = 0;
	static bool do_ = false;

	if (Cfg.FakeDuck && Cfg.InfiniteDuck && InputSys::Get().IsKeyDown(Cfg.FakeDuck_Bind)) {
		g_LocalPlayer->GetPlayerAnimState()->m_fDuckAmount = 1.f;

		bSendPacket = false;

		if (cnt % 14 == 0)
			do_ = true;
		else if (cnt % 14 == 6)
			bSendPacket = true;
		else if (cnt % 14 == 7)
			do_ = false;

		if (do_) {
			duck_moment = 1;
			cmd->buttons |= IN_DUCK;
		}
		else {
			cmd->buttons &= ~IN_DUCK;
			duck_moment = 0;
		}

		cnt++;
	}
	else {
		do_ = false;
		cnt = 0;
	}
}
//--------------------------------------------------------------------------------------------------
void MovementFix::Start(CUserCmd* cmd) {
	m_oldangle = cmd->viewangles;
	m_oldforward = cmd->forwardmove;
	m_oldsidemove = cmd->sidemove;
}
//--------------------------------------------------------------------------------------------------
void MovementFix::End(CUserCmd* cmd) {
	float yaw_delta = cmd->viewangles.yaw - m_oldangle.yaw;
	float f1;
	float f2;

	if (m_oldangle.yaw < 0.f) f1 = 360.0f + m_oldangle.yaw;
	else f1 = m_oldangle.yaw;

	if (cmd->viewangles.yaw < 0.0f) f2 = 360.0f + cmd->viewangles.yaw;
	else f2 = cmd->viewangles.yaw;

	if (f2 < f1) yaw_delta = abs(f2 - f1);
	else yaw_delta = 360.0f - abs(f1 - f2);
	yaw_delta = 360.0f - yaw_delta;

	cmd->forwardmove = cos(DEG2RAD(yaw_delta)) * m_oldforward + cos(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
	cmd->sidemove = sin(DEG2RAD(yaw_delta)) * m_oldforward + sin(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
}
/*
//--------------------------------------------------------------------------------------------------
void RageAimbot::StartEnginePred(CUserCmd* cmd) {
	static int nTickBase;
	static CUserCmd* pLastCmd;

	if (pLastCmd) {
		if (pLastCmd->hasbeenpredicted) 
			nTickBase = g_LocalPlayer->m_nTickBase();
		else
			++nTickBase;
	}

	pLastCmd = cmd;
	flOldCurtime = g_GlobalVars->curtime;
	flOldFrametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = float(nTickBase) * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(g_LocalPlayer);

	CMoveData data;
	memset(&data, 0, sizeof(CMoveData));
	g_MoveHelper->SetHost(g_LocalPlayer);
	g_Prediction->SetupMove(g_LocalPlayer, cmd, g_MoveHelper, &data);
	g_GameMovement->ProcessMovement(g_LocalPlayer, &data);
	g_Prediction->FinishMove(g_LocalPlayer, cmd, &data);
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::EndEnginePred() {
	g_GameMovement->FinishTrackPredictionErrors(g_LocalPlayer);
	g_MoveHelper->SetHost(nullptr);

	g_GlobalVars->curtime = flOldCurtime;
	g_GlobalVars->frametime = flOldFrametime;
}

//--------------------------------------------------------------------------------------------------*/
//--------------------------------------------------------------------------------------------------
inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
#define square( x ) ( x * x )
void ClampMovement(CUserCmd* pCommand, float fMaxSpeed)
{
	if (fMaxSpeed <= 0.f)
		return;
	float fSpeed = (float)(FastSqrt(square(pCommand->forwardmove) + square(pCommand->sidemove) + square(pCommand->upmove)));
	if (fSpeed <= 0.f)
		return;
	if (pCommand->buttons & IN_DUCK)
		fMaxSpeed *= 2.94117647f;
	if (fSpeed <= fMaxSpeed)
		return;
	float fRatio = fMaxSpeed / fSpeed;
	pCommand->forwardmove *= fRatio;
	pCommand->sidemove *= fRatio;
	pCommand->upmove *= fRatio;
}
bool RageAimbot::Hitchance(C_BaseCombatWeapon* weapon, QAngle angles, C_BasePlayer* ent, float chance)
{
	auto weapon1 = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon1)
		return false;

	Vector forward, right, up;
	Vector src = g_LocalPlayer->GetEyePos();
	Math::AngleVectors(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int> (150.f * (chance / 100.f));

	weapon1->UpdateAccuracyPenalty();
	float weap_spread = weapon1->GetSpread();
	float weap_inaccuracy = weapon1->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = Math::RandomFloat(0.f, 1.f);
		float b = Math::RandomFloat(0.f, 2.f * PI);
		float c = Math::RandomFloat(0.f, 1.f);
		float d = Math::RandomFloat(0.f, 2.f * PI);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon1->m_Item().m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		Math::VectorAngles1337(direction, up, viewAnglesSpread);
		viewAnglesSpread.Normalize();

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon1->GetCSWeaponData()->flRange);

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int> ((static_cast<float> (cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}

	return false;
}
//--------------------------------------------------------------------------------------------------
std::vector<int> RageAimbot::GetHitboxes()
{
	std::vector<int> hitboxes;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && Cfg.baim_onair || //on air
		InputSys::Get().IsKeyDown(Cfg.SlowWalk_Bind) && Cfg.baim_whenslowwalk || //when slowwalk
		InputSys::Get().IsKeyDown(Cfg.baim_onkey_key) && Cfg.baim_onkey //on key(gay)
		) {
		hitboxes.push_back((int)Hitboxes::HITBOX_UPPER_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_LOWER_CHEST);
	}
	else {
		if (Cfg.rage_hitscan1) hitboxes.push_back((int)Hitboxes::HITBOX_HEAD);
		if (Cfg.rage_hitscan2) hitboxes.push_back((int)Hitboxes::HITBOX_NECK);
		if (Cfg.rage_hitscan3)	{
			hitboxes.push_back((int)Hitboxes::HITBOX_UPPER_CHEST);
			hitboxes.push_back((int)Hitboxes::HITBOX_CHEST);
			hitboxes.push_back((int)Hitboxes::HITBOX_LOWER_CHEST);
		}
		if (Cfg.rage_hitscan4) hitboxes.push_back((int)Hitboxes::HITBOX_STOMACH);
		if (Cfg.rage_hitscan5) hitboxes.push_back((int)Hitboxes::HITBOX_PELVIS);
		if (Cfg.rage_hitscan6)		{
			hitboxes.push_back(18);
			hitboxes.push_back(16);
			hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_UPPER_ARM);
			hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_UPPER_ARM);
			hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_HAND);
			hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_HAND);
		}
		if (Cfg.rage_hitscan7)	{
			hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_CALF);
			hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_CALF);
			hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_THIGH);
			hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_THIGH);
			hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_FOOT);
			hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_FOOT);
		}
	}

	return hitboxes;
} 
//--------------------------------------------------------------------------------------------------
bool RageAimbot::Hitscan(C_BasePlayer* pEntity, Vector& HitboxPos, bool Backtrack, matrix3x4_t* BoneMatrix)
{
	//	std::vector<int> HitBoxesToScan{ HITBOX_HEAD, HITBOX_NECK, HITBOX_CHEST, HITBOX_STOMACH, HITBOX_LEFT_THIGH, HITBOX_RIGHT_THIGH };

	auto HitBoxesToScan = this->GetHitboxes();

	int bestHitbox = -1;

	if (!Backtrack) {
		float highestDamage;

		highestDamage = float(Cfg.Damage);

		for (int HitBoxID : HitBoxesToScan) {
			pEntity->SetAbsOrigin(pEntity->m_vecOrigin());
			Vector Point = pEntity->GetHitboxPos(HitBoxID, BoneMatrix);
			float damage = Autowall::Get().CanHit(Point);
			if (damage >= highestDamage || damage >= pEntity->m_iHealth()) {
				bestHitbox = HitBoxID;
				highestDamage = damage;
				HitboxPos = Point;
				return true;
			}
		}
	}
	else {
		for (int HitBoxID : HitBoxesToScan) {
			Vector Point = pEntity->GetHitboxPos(HitBoxID, BoneMatrix);

			if (g_LocalPlayer->CanSeePlayer(pEntity, Point)) {
				bestHitbox = HitBoxID;
				HitboxPos = Point;
				return true;
			}
		}
	}

	return bestHitbox;
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::SlowWalk(CUserCmd* cmd) {

	if (Cfg.slowwalk_speed <= 0 || !InputSys::Get().IsKeyDown(Cfg.SlowWalk_Bind))
		return;

	auto weapon_handle = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon_handle)
		return;

	float amount = 0.0034f * Cfg.slowwalk_speed;

	Vector velocity = g_LocalPlayer->m_vecVelocity();
	QAngle direction;

	Math::VectorAngles(velocity, direction);

	float speed = velocity.Length2D();

	direction.yaw = cmd->viewangles.yaw - direction.yaw;

	Vector forward;

	Math::AngleVectors(direction, forward);

	Vector source = forward * -speed;

	if (speed >= (weapon_handle->GetCSWeaponData()->flMaxPlayerSpeed * amount)) {
		cmd->forwardmove = source.x;
		cmd->sidemove = source.y;
	}
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::StoreRecords() {
	for (int i = 1; i <= 64; i++) {
		auto pEntity = C_BasePlayer::GetPlayerByIndex(i);

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsPlayer() || !pEntity->IsAlive() || !pEntity->IsEnemy()) {
			if (!BacktrackRecords[i].empty())
				for (int Tick = 0; Tick < BacktrackRecords[i].size(); Tick++) BacktrackRecords[i].erase(BacktrackRecords[i].begin() + Tick);
			continue;
		}

		BacktrackRecords[i].insert(BacktrackRecords[i].begin(), TickInfo(pEntity));
		for (auto Tick : BacktrackRecords[i]) if (!Utilities::IsTickValid(Tick.SimulationTime, 0.2f)) BacktrackRecords[i].pop_back();
	}
}
bool RageAimbot::AutoRevolver(CUserCmd* cmd)
{
	if (g_LocalPlayer->m_hActiveWeapon().Get()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
		static int delay = 0;
		delay++;
		if (delay <= 15) cmd->buttons |= IN_ATTACK;
		else delay = 0;
	}

	return true;
}
void RageAimbot::Autostop(CUserCmd* cmd)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		if (weapon->IsGrenade() || weapon->IsKnife() || weapon->m_iItemDefinitionIndex() == WEAPON_TASER) //we dont wanna stop if we holdin a knife, grenade or zeus
			return;
		ClampMovement(cmd, 2);
	}
}
//--------------------------------------------------------------------------------------------------
float Hitchance2(C_BaseCombatWeapon* Weapon) {
	float Hitchance = 101;

	if (!Weapon) return 0;
	if (Cfg.Hitchance > 1) {
		float Inaccuracy = Weapon->GetInaccuracy();

		if (Inaccuracy == 0) Inaccuracy = 0.0000001;
		Inaccuracy = 1 / Inaccuracy;
		Hitchance = Inaccuracy;
	}
	return Hitchance;
}
//--------------------------------------------------------------------------------------------------
void Backtrack::Do(CUserCmd* cmd)
{
	if (!g_LocalPlayer ||
		!g_LocalPlayer->IsAlive())
		return;

	int BacktrackedPlayer = -1;
	float MaxPlayerFov = INT_MAX;

	for (int i = 1; i <= 64; i++)
	{
		C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);
		if (!Player || Player->IsDormant() || !Player->IsPlayer() || !Player->IsAlive() || !Player->IsEnemy())
		{
			if (BacktrackRecords[i].size() > 0)
				for (int Tick = 0; Tick < BacktrackRecords[i].size(); Tick++)
					BacktrackRecords[i].erase(BacktrackRecords[i].begin() + Tick);
			continue;
		}

		BacktrackRecords[i].insert(BacktrackRecords[i].begin(), TickInfo(Player));
		for (auto Tick : BacktrackRecords[i])
			if (!Utilities::IsTickValid(Tick.SimulationTime, float(Cfg.rage_backtrack_time) / 1000.f))
				BacktrackRecords[i].pop_back();

		Vector ViewAngles;
		Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat()), ViewAngles);
		float FovDistance = Math::DistancePointToLine(Player->GetHitboxPos(HITBOX_HEAD), g_LocalPlayer->GetEyePos(), ViewAngles);

		if (MaxPlayerFov > FovDistance)
		{
			MaxPlayerFov = FovDistance;
			BacktrackedPlayer = i;
		}
	}
	ClosestTick = -1;
	float MaxTickFov = INT_MAX;
	if (BacktrackedPlayer != -1)
	{
		for (int t = 0; t < BacktrackRecords[BacktrackedPlayer].size(); t++)
		{
			Vector ViewAngles2;
			Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat()), ViewAngles2);
			float FovDistance2 = Math::DistancePointToLine(BacktrackRecords[BacktrackedPlayer].at(t).HeadPosition, g_LocalPlayer->GetEyePos(), ViewAngles2);

			if (MaxTickFov > FovDistance2)
			{
				MaxTickFov = FovDistance2;
				ClosestTick = t;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::Do(CUserCmd* cmd, C_BaseCombatWeapon* Weapon, bool& bSendPacket)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !Weapon || Weapon->IsKnife() || Weapon->IsGrenade() || !Cfg.RageEnabled)
		return;

	int BestTargetIndex = -1;
	float BestTargetDistance = FLT_MAX;
	float BestTargetSimtime = 0.f;

	Vector Hitbox = Vector{};

	bool Backtrack = false;

	for (int i = 1; i <= 64; i++)
	{
		auto pEntity = C_BasePlayer::GetPlayerByIndex(i);
		if (!pEntity || !pEntity->IsPlayer() || pEntity->IsDormant() || !pEntity->IsAlive() || !pEntity->IsEnemy() || BacktrackRecords[i].empty()) continue;

		float PlayerDistance = Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), pEntity->m_vecOrigin());

		if (BestTargetDistance > PlayerDistance)
		{
			if (BacktrackRecords[i].front().MatrixBuilt && BacktrackRecords[i].front().BoneMatrix != nullptr &&
				Hitscan(pEntity, Hitbox, false, BacktrackRecords[i].front().BoneMatrix))
			{
				BestTargetDistance = PlayerDistance;
				BestTargetIndex = i;
				BestTargetSimtime = pEntity->m_flSimulationTime();
				Backtrack = false;
			}
			else if (BacktrackRecords[i].back().MatrixBuilt && BacktrackRecords[i].back().BoneMatrix != nullptr &&
				Hitscan(pEntity, Hitbox, true, BacktrackRecords[i].back().BoneMatrix))
			{
				BestTargetDistance = PlayerDistance;
				BestTargetIndex = i;
				BestTargetSimtime = BacktrackRecords[i].back().SimulationTime;
				Backtrack = true;
			}
		}
	}
	if (BestTargetIndex != -1 && Hitbox.IsValid() && BestTargetSimtime) 
	{

		const auto local_weapon = g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex();
		
		if (local_weapon == WEAPON_SCAR20 || local_weapon == WEAPON_G3SG1 || local_weapon == WEAPON_SSG08 || local_weapon == WEAPON_AWP) {
			if (!g_LocalPlayer->m_bIsScoped() && Cfg.AutoScope) {
				cmd->buttons |= IN_ATTACK2;
				if (Cfg.autostop)
					Autostop(cmd);
			}
		}

			if (Cfg.autostop)
				Autostop(cmd);

			if (Cfg.autocrouch)
				cmd->buttons |= IN_DUCK;

			auto Target = C_BasePlayer::GetPlayerByIndex(BestTargetIndex);
			if (!Target) return;
			QAngle AimAngle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), Hitbox);
			AimAngle -= g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat();
			Math::Normalize(AimAngle);
			Math::ClampAngles(AimAngle);

			cmd->viewangles = AimAngle;

			C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

			if (Weapon && Weapon->AWP())  //AWP
			{
				Cfg.Hitchance = Cfg.AwpHitchance;
				Cfg.Damage = Cfg.AwpDamage;
			}

			if (Weapon && Weapon->AutoScar()) //AutoScar
			{
				Cfg.Hitchance = Cfg.AutoHitchance;
				Cfg.Damage = Cfg.AutoDamage;
			}

			if (Weapon && Weapon->Scout()) //Scout
			{
				Cfg.Hitchance = Cfg.ScoutHitchance;
				Cfg.Damage = Cfg.ScoutDamage;
			}

			if (Weapon && Weapon->IsRifle())
			{
				Cfg.Hitchance = Cfg.RifleHitchance;
				Cfg.Damage = Cfg.RifleDamage;
			}

			if (Weapon && Weapon->IsPistol())  //PistolLite
			{
				Cfg.Hitchance = Cfg.PistolHitchance;
				Cfg.Damage = Cfg.PistolDamage;
			}

			if (Weapon && Weapon->H_Pistol()) //H_Pistol
			{
				Cfg.Hitchance = Cfg.HeavyPistHitchance;
				Cfg.Damage = Cfg.HeavyPistDamage;
			}


			if (Hitchance(Weapon, cmd->viewangles, Target, float(Cfg.Hitchance)) || Backtrack && Cfg.Hitchance * 1.5 <= Hitchance2(Weapon) || Cfg.Hitchance == 0) {
				if (!(cmd->buttons & IN_ATTACK) && Weapon->CanFire()) {
					bSendPacket = true;
					cmd->tick_count = TIME_TO_TICKS(BestTargetSimtime + Utilities::GetLerpTime());
					cmd->buttons |= IN_ATTACK;
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	bool LbyUpdate()
	{

		auto speed = g_LocalPlayer->m_vecVelocity().Length2D();
		static float next_lby = 0.00f;
		float curtime = g_GlobalVars->curtime;

		if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			return false;

		if (speed > 0.1f)
			next_lby = curtime + 0.22;

		if (next_lby < curtime)
		{
			next_lby = curtime + 1.1;
			return true;
		}
		else
			return false;
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::Desync(CUserCmd* cmd, bool& bSendPacket)
{
	bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f || (cmd->sidemove != 0.f || cmd->forwardmove != 0.f);
	bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
	bool Standing = !Moving && !InAir;

	int FakeLagTicks = Cfg.FakelagChoke;

	if (FakeLagTicks == 0)
	{
		static bool sw = false;
		bSendPacket = sw;
		sw = !sw;
	}

	static QAngle LastRealAngle = QAngle(0, 0, 0);

	if (!bSendPacket && !(cmd->buttons & IN_ATTACK))
	{
		static bool bFlip = false;
		cmd->viewangles.yaw += bFlip ? 58.f : -58.f;
	}

	if (bSendPacket)
	{
		LastRealAngle = cmd->viewangles;
	}
}
//--------------------------------------------------------------------------------------------------
enum MovingAAType {
	Standing = 0,
	Moving,
	Air
};
int ReturnMovementType() {
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
		if (g_LocalPlayer->GetVelocity().Length2D() < 32) {
			return MovingAAType::Standing;
		}
		else if (g_LocalPlayer->GetVelocity().Length2D() > 32) {
			return MovingAAType::Moving;
		}
	}
	else {
		return MovingAAType::Air;
	}
}
//--------------------------------------------------------------------------------------------------
void RageAimbot::DoAntiaim(CUserCmd* cmd, C_BaseCombatWeapon* Weapon, bool& bSendPacket) {
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !Weapon || Weapon->IsKnife() || cmd->buttons & IN_ATTACK && Weapon->CanFire() || cmd->buttons & IN_USE || Weapon->IsGrenade() || g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	static int AASideStand = 1;
	static int AASideMov = 1;
	static int AASideAir = 1;

	if (InputSys::Get().IsKeyDown(Cfg.AALeftSideBind_stand))
		AASideStand = 0;
	if (InputSys::Get().IsKeyDown(Cfg.AABackSideBind_stand))
		AASideStand = 1;
	if (InputSys::Get().IsKeyDown(Cfg.AARightSideBind_stand))
		AASideStand = 2;

	if (InputSys::Get().IsKeyDown(Cfg.AALeftSideBind_mov))
		AASideMov = 0;
	if (InputSys::Get().IsKeyDown(Cfg.AABackSideBind_mov))
		AASideMov = 1;
	if (InputSys::Get().IsKeyDown(Cfg.AARightSideBind_mov))
		AASideMov = 2;

	if (InputSys::Get().IsKeyDown(Cfg.AALeftSideBind_air))
		AASideAir = 0;
	if (InputSys::Get().IsKeyDown(Cfg.AABackSideBind_air))
		AASideAir = 1;
	if (InputSys::Get().IsKeyDown(Cfg.AARightSideBind_air))
		AASideAir = 2;

	switch (ReturnMovementType()) {
	case MovingAAType::Standing: {
		switch (Cfg.pitch_standing) {
		case 0: return;
		case 1: cmd->viewangles.pitch = 88.69f; break;
		case 2: cmd->viewangles.pitch = 90.f; break;
		case 3: cmd->viewangles.pitch = -88.69f; break;
		case 4: cmd->viewangles.pitch = 1080.f; break;
		case 5: cmd->viewangles.pitch = Cfg.aapitch_cust_stand; break;
		}
	}break;
	case MovingAAType::Moving: {
		switch (Cfg.pitch_moving) {
		case 0: return;
		case 1: cmd->viewangles.pitch = 88.69f; break;
		case 2: cmd->viewangles.pitch = 90.f; break;
		case 3: cmd->viewangles.pitch = -88.69f; break;
		case 4: cmd->viewangles.pitch = 1080.f; break;
		case 5: cmd->viewangles.pitch = Cfg.aapitch_cust_mov; break;
		}
	}break;
	case MovingAAType::Air: {
		switch (Cfg.pitch_air) {
		case 0: return;
		case 1: cmd->viewangles.pitch = 88.69f; break;
		case 2: cmd->viewangles.pitch = 90.f; break;
		case 3: cmd->viewangles.pitch = -88.69f; break;
		case 4: cmd->viewangles.pitch = 1080.f; break;
		case 5: cmd->viewangles.pitch = Cfg.aapitch_cust_air; break;
		}
	}break;
	}

	//float_t fix = cmd->viewangles.yaw;

	switch (ReturnMovementType())
	{
	case MovingAAType::Standing:
	{
		switch (Cfg.yaw_standing)
		{
		case 0: return;
		case 1: cmd->viewangles.yaw += 180.0f + Cfg.yawadd_stand; break;
		case 2: cmd->viewangles.yaw += Math::RandomFloat(-Cfg.jitter_radius_stand, Cfg.jitter_radius_stand) + Cfg.yawadd_stand; break;
		case 3: cmd->viewangles.yaw += 90.f + 90.f * AASideStand + Cfg.yawadd_stand; break;
		case 4: cmd->viewangles.yaw += fmodf(g_GlobalVars->tickcount * 10.f, 360.f); break;
		}
		break;
	case MovingAAType::Moving:
	{
		switch (Cfg.yaw_moving)
		{
		case 0: return;
		case 1:
			cmd->viewangles.yaw += 180.0f + Cfg.yawadd_mov;
			break;
		case 2: cmd->viewangles.yaw += Math::RandomFloat(-Cfg.jitter_radius_mov, Cfg.jitter_radius_mov) + Cfg.yawadd_mov; break;
		case 3: cmd->viewangles.yaw += 90.f + 90.f * AASideMov + Cfg.yawadd_mov; break;
		case 4: cmd->viewangles.yaw += fmodf(g_GlobalVars->tickcount * 10.f, 360.f); break;
		}
	}
	break;
	case MovingAAType::Air:
	{
		switch (Cfg.yaw_air)
		{
		case 0: return;
		case 1: cmd->viewangles.yaw += 180.0f + Cfg.yawadd_air; break;
		case 2: cmd->viewangles.yaw += Math::RandomFloat(-Cfg.jitter_radius_air, Cfg.jitter_radius_air) + Cfg.yawadd_air; break;
		case 3: cmd->viewangles.yaw += 90.f + 90.f * AASideAir + Cfg.yawadd_air; break;
		case 4: cmd->viewangles.yaw += fmodf(g_GlobalVars->tickcount * 10.f, 360.f); break;
		}
	}
	break;
	}
	if (LbyUpdate()) 
	{
		bSendPacket = false;
		cmd->viewangles.yaw -= 125.f;
		return;
	}
	if (bSendPacket)
		cmd->viewangles.yaw += g_LocalPlayer->MaxDesyncDelta();
	cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
	}
}