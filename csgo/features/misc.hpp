#pragma once
#include "../valve_sdk/structs.hpp"
#include "../singleton.hpp"

enum class BuyPistols : int
{
	NONE,
	GLOCK_18__USP_S__P2000,
	DUAL_BERETTAS,
	P250,
	TEC_9__FIVE_SEVEN,
	DESERT_EAGLE__R8
};

enum class BuyWeapons : int
{
	NONE,
	SG_AUG,
	SSG,
	AUTO,
	MAC_10,
	P90,
	PP_BIZON,
	AK_47,
	AWP
};

class Misc : public Singleton<Misc>
{
public:
	void OnRoundStart();
	void OnCreateMove();
	void Clantag();
	void ChatSpam();
	void DisablePanoramaBlur();
	void No3dSky();
	void NoShadows();
	void DisablePostProcessing();
	void Nightmode();
	void LegitAA(CUserCmd* cmd, bool bSendPacket);
private:
	float LastRoundStartTime = -1.f;
	bool ShouldBuy = false;
	CBasePlayerAnimState AnimState;
};
