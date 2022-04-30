#pragma once

class CLagCompensation
{

private:

	struct CLagRecord
	{
		struct
		{
			int m_nOrder = 0;
			int m_nSequence = 0;
			float m_flWeight = 0.f;
			float m_flCycle = 0.f;
		} m_Layers;
	};

public:

	enum class CProxie
	{
		PROXY_EYEANGLES_0 = 0,
		PROXY_EYEANGLES_1
	};

	float GetLerpTime( );

	void OnMove( void* m_pCmd, bool* m_pbSendPacket );
	void OnFrameStageNotify( size_t m_nStage );
	void OnProxies( CProxie m_nProxy );

private:

	void StartLagCompensation( );

	CLagRecord* m_pLagRecord = nullptr;

	float m_flLerpTime = 0.f;

};

