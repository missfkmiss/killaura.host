#include "LagCompensation.hpp"

#include "../valve_sdk/sdk.hpp"

#include <algorithm>

float CLagCompensation::GetLerpTime( )
{
	static float m_flInterp = g_CVar->FindVar( "cl_interp" )->GetFloat( );
	static float m_flInterpRatio = g_CVar->FindVar( "cl_interp_ratio" )->GetFloat( );
	static float m_flMinRatio = g_CVar->FindVar( "sv_client_min_interp_ratio" )->GetFloat( );
	static float m_flMaxRatio = g_CVar->FindVar( "sv_client_max_interp_ratio" )->GetFloat( );
	static float m_flUpdateRate = g_CVar->FindVar( "cl_updaterate" )->GetFloat( );
	static float m_flMinUpdateRate = g_CVar->FindVar( "sv_minupdaterate" )->GetFloat( );
	static float m_flMaxUpdateRate = g_CVar->FindVar( "sv_maxupdaterate" )->GetFloat( );

	float m_flFinalRate = std::clamp( m_flUpdateRate, m_flMaxUpdateRate, m_flMinUpdateRate );
	float m_flFinalRatio = std::clamp( m_flInterpRatio, m_flMinRatio, m_flMaxRatio );

	return std::clamp( m_flFinalRatio / m_flFinalRate, m_flInterp, 1.f );
}

void CLagCompensation::OnMove( void* m_pCmdRC, bool* m_pbSendPacket )
{
	CUserCmd* m_pCmd = reinterpret_cast< CUserCmd* >( m_pCmdRC );
	if ( !m_pCmd )
		return;
}

void CLagCompensation::OnFrameStageNotify( size_t m_nStage )
{
	switch ( static_cast< ClientFrameStage_t >( m_nStage ) )
	{
		case FRAME_RENDER_START:
		{
			// todo
			break;
		}
		default:
			return;
	}
}

void CLagCompensation::OnProxies( CProxie m_nProxy )
{
}

void CLagCompensation::StartLagCompensation( )
{
	if ( !m_pLagRecord )
		m_pLagRecord = new CLagRecord( );

	// todo
}