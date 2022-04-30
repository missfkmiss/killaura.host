#pragma once
#include "../options.hpp"
#include "../valve_sdk/structs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/Utilities.hpp"
#include "../singleton.hpp"
#include "../helpers/input.hpp"

class FakeLag : public Singleton<FakeLag>
{
	friend class Singleton<FakeLag>;
public:
	void FakeDuck(CUserCmd* cmd, bool& bSendPacket);
	int duck_moment;
};

class MovementFix : public Singleton<MovementFix>
{
	friend class Singleton<MovementFix>;
public:
	void Start(CUserCmd* cmd);
	void End(CUserCmd* cmd);
private:
	float m_oldforward, m_oldsidemove;
	QAngle m_oldangle;
};

class Backtrack :
	public Singleton<Backtrack>
{
	friend class Singleton<Backtrack>;
private:
	struct TickInfo
	{
		TickInfo()
		{
			HeadPosition = Vector{};
			SimulationTime = -1;
			MatrixBuilt = false;
		}
		TickInfo(C_BasePlayer* Player)
		{
			HeadPosition = Player->GetHitboxPos(HITBOX_HEAD);
			SimulationTime = Player->m_flSimulationTime();
			MatrixBuilt = false;
			if (Player->SetupBones(BoneMatrix, 128, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
				MatrixBuilt = true;
		}
		Vector HeadPosition;
		float SimulationTime;
		bool MatrixBuilt;
		matrix3x4_t BoneMatrix[128];
	};
public:
	void Do(CUserCmd* cmd);
	std::vector<TickInfo> BacktrackRecords[65];
	int ClosestTick;
};

class RageAimbot : public Singleton<RageAimbot>
{
	friend class Singleton<RageAimbot>;
private:
	struct TickInfo
	{
		TickInfo()
		{
			SimulationTime = 0.f;
			Origin = Vector{};
			MatrixBuilt = false;
		}
		TickInfo(C_BasePlayer* Player)
		{
			SimulationTime = Player->m_flSimulationTime();
			Origin = Player->m_vecOrigin();
			MatrixBuilt = false;
			if (Player->SetupBones(BoneMatrix, 128, BONE_USED_BY_HITBOX, g_GlobalVars->curtime)) MatrixBuilt = true;
		}
		float SimulationTime;
		Vector Origin;
		bool MatrixBuilt;
		matrix3x4_t BoneMatrix[128];
	};
	bool Hitscan(C_BasePlayer* Player, Vector& HitboxPos, bool Backtrack, matrix3x4_t* BoneMatrix);
	bool Hitchance(C_BaseCombatWeapon* weapon, QAngle angles, C_BasePlayer* ent, float chance);
	//bool Hitchance_test(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, Vector Point, int chance);
	std::vector<int> GetHitboxes();
public:
	void StoreRecords();
	bool AutoRevolver(CUserCmd* cmd);
	std::vector<TickInfo> BacktrackRecords[65];
	void Autostop(CUserCmd* cmd);
	//void StartEnginePred(CUserCmd* cmd);
	//void EndEnginePred();
	void SlowWalk(CUserCmd* cmd);
	void Do(CUserCmd* cmd, C_BaseCombatWeapon* Weapon, bool& bSendPacket);
	void Desync(CUserCmd* cmd, bool& bSendPacket);
	void DoAntiaim(CUserCmd* cmd, C_BaseCombatWeapon* Weapon, bool& bSendPacket);
};
