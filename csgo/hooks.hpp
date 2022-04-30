#pragma once
#include "valve_sdk/structs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>

namespace index
{
	constexpr auto EndScene					= 42;
	constexpr auto Reset					= 16;
    constexpr auto PaintTraverse            = 41;
    constexpr auto CreateMove               = 22;
    constexpr auto FrameStageNotify         = 37;
    constexpr auto DrawModelExecute         = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool          = 13;
	constexpr auto OverrideView             = 18;
	constexpr auto EngineHook				= 27;
	constexpr auto LockCursor               = 67;
	constexpr auto GetViewmodelFOV			= 35;
	constexpr auto FireEvent                = 9;
	constexpr auto RunCommand = 19;
}

namespace Hooks
{
	void Initialize();

	extern float RealAngle;
	extern float FakeAngle;
	extern float Pitch;

	inline QAngle MoY_UgOl;
	extern vfunc_hook direct3d_hook;
	extern vfunc_hook hlclient_hook;
	extern vfunc_hook vguipanel_hook;
	extern vfunc_hook vguisurf_hook;
	extern vfunc_hook mdlrender_hook;
	extern vfunc_hook viewrender_hook;
	extern vfunc_hook enginehook_hook;
	extern vfunc_hook clientmode_hook;

	using GetViewmodelFOV = float(__thiscall*)();
	using IsConnected_t = bool(__thiscall*)(IVEngineClient*);
	using FireEvent = bool(__thiscall*)(IGameEventManager2*, IGameEvent * pEvent);

	using RunCommand_t = void( __thiscall* )( IPrediction*, C_BasePlayer*, CUserCmd*, IMoveHelper* );

	long __stdcall hkEndScene(IDirect3DDevice9* device);
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
	void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active);
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	void __fastcall hkDrawModelExecute(void* _this, int, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __fastcall hkFrameStageNotify(void* _this, int, ClientFrameStage_t stage);
	void __fastcall hkOverrideView(void* _this, int, CViewSetup * vsView);
	int  __fastcall hkDoPostScreenEffects(void* _this, int, int a1);
	void __fastcall hkLockCursor(void* _this);
	bool __stdcall hkFireEvent(IGameEvent* pEvent);
	void __fastcall hkRunCommand( IPrediction* m_pPrediction, uint32_t, C_BasePlayer* m_pPlayer, CUserCmd* m_pCmd, IMoveHelper* m_pMoveHelper );

	namespace Proxies
	{
		void hkEyeAngles_0( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult );
		void hkEyeAngles_1( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult );
		void hkFlashDuration( const CRecvProxyData* m_pData, void* m_pStruct, void* m_pResult );
	}
	bool __fastcall hkWriteUsercmdDeltaToBuffer(void* m_pClient, void*, int m_nSlot, void* m_pBuffer, int m_nFrom, int m_nTo, bool m_bNewCmd);
}