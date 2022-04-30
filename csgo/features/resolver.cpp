#include "Resolver.hpp"
#include "Autowall.hpp"
#include "../valve_sdk/structs.hpp"
#include "../helpers/math.hpp"

void Resolver::Resolve()
{
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		auto& record = arr_infos[i];
		if (!record.m_bActive)
			continue;

		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);
		if (!player || !player->IsAlive() || player->IsDormant() || player == g_LocalPlayer)
			continue;

		if (record.m_flVelocity == 0.f && player->m_vecVelocity().Length2D() != 0.f)
		{
			Math::VectorAngles(player->m_vecVelocity(), record.m_angDirectionFirstMoving);
			record.m_nCorrectedFakewalkIdx = 0;
		}

		auto firedShots = g_LocalPlayer->m_iShotsFired();

		AnimationLayer curBalanceLayer, prevBalanceLayer;

		ResolveInfo curtickrecord;
		curtickrecord.SaveRecord(player);

		if ((player->m_fFlags() & FL_ONGROUND) && (IsFakewalking(player, curtickrecord) || (player->m_vecVelocity().Length2D() > 0.1f && player->m_vecVelocity().Length2D() < 45.f && !(player->m_fFlags() & FL_DUCKING)))) //Fakewalk, shiftwalk check // We have to rework the fakewalk resolving, it sucks :D
		{
			float_t new_yaw = ResolveFakewalk(player, curtickrecord);
			new_yaw = Math::ClampYaw(new_yaw);

			player->EyeAngles().yaw = new_yaw;

			continue;
		}
		if (IsEntityMoving(player))
		{
			float_t new_yaw = player->m_flLowerBodyYawTarget();
			new_yaw = Math::ClampYaw(new_yaw);

			player->EyeAngles().yaw = new_yaw;

			record.m_flStandingTime = player->m_flSimulationTime();
			record.m_flMovingLBY = player->m_flLowerBodyYawTarget();
			record.m_bIsMoving = true;

			continue;
		}
		if (IsAdjustingBalance(player, curtickrecord, &curBalanceLayer))
		{
			if (fabsf(LBYDelta(curtickrecord)) > 35.f)
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
				new_yaw = Math::ClampYaw(new_yaw);

				player->EyeAngles().yaw = new_yaw;
			}
			if (IsAdjustingBalance(player, record, &prevBalanceLayer))
			{
				if ((prevBalanceLayer.m_flCycle != curBalanceLayer.m_flCycle) || curBalanceLayer.m_flWeight == 1.f)
				{
					float
						flAnimTime = curBalanceLayer.m_flCycle,
						flSimTime = player->m_flSimulationTime();

					float_t new_yaw = player->m_flLowerBodyYawTarget();
					new_yaw = Math::ClampYaw(new_yaw);

					player->EyeAngles().yaw = new_yaw;

					continue;
				}
				else if (curBalanceLayer.m_flWeight == 0.f && (prevBalanceLayer.m_flCycle > 0.92f && curBalanceLayer.m_flCycle > 0.92f)) // breaking lby with delta < 120
				{
					if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
					{
						record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

						float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
						new_yaw = Math::ClampYaw(new_yaw);

						player->EyeAngles().yaw = new_yaw;

						record.m_bIsMoving = false;

						continue;
					}

					if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
					{
						record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
						float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
						new_yaw = Math::ClampYaw(new_yaw);

						player->EyeAngles().yaw = new_yaw;

						record.m_bIsMoving = false;

						continue;
					}
				}
			}
			else
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget();
				new_yaw = Math::ClampYaw(new_yaw);

				player->EyeAngles().yaw = new_yaw;

				continue;
			}
		}
		if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
		{
			record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

			float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
			new_yaw = Math::ClampYaw(new_yaw);

			player->EyeAngles().yaw = new_yaw;

			record.m_bIsMoving = false;

			continue;
		}
		if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
		{
			record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
			float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
			new_yaw = Math::ClampYaw(new_yaw);

			player->EyeAngles().yaw = new_yaw;

			record.m_bIsMoving = false;

			continue;
		}

		float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
		new_yaw = Math::ClampYaw(new_yaw);

		player->EyeAngles().yaw = new_yaw;
	}
}
float_t Resolver::ResolveFakewalk(C_BasePlayer* player, ResolveInfo& curtickrecord)
{
	auto& record = arr_infos[player->EntIndex()];

	float_t yaw;
	int32_t correctedFakewalkIdx = record.m_nCorrectedFakewalkIdx;

	if (correctedFakewalkIdx < 2)
	{
		yaw = record.m_angDirectionFirstMoving.yaw + 180.f;
	}
	else if (correctedFakewalkIdx < 4)
	{
		yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
	}
	else if (correctedFakewalkIdx < 6)
	{
		yaw = record.m_angDirectionFirstMoving.yaw;
	}
	else
	{
		QAngle dir;
		Math::VectorAngles(curtickrecord.m_vecVelocity, dir);

		yaw = dir.yaw;
	}

	return yaw;
}
bool Resolver::IsEntityMoving(C_BasePlayer* player)
{
	return (player->m_vecVelocity().Length2D() > 0.1f && player->m_fFlags() & FL_ONGROUND);
}
bool Resolver::IsAdjustingBalance(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 979)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

bool Resolver::IsAdjustingStopMoving(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 980)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}
bool Resolver::IsFakewalking(C_BasePlayer* player, ResolveInfo& record)
{
	bool
		bFakewalking = false,
		stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;

	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		if (record.animationLayer[i].m_nSequence == 26 && record.animationLayer[i].m_flWeight < 0.4f)
			stage1 = true;
		if (record.animationLayer[i].m_nSequence == 7 && record.animationLayer[i].m_flWeight > 0.001f)
			stage2 = true;
		if (record.animationLayer[i].m_nSequence == 2 && record.animationLayer[i].m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		if (stage3 || (player->m_fFlags() & FL_DUCKING)) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
			bFakewalking = true;
		else
			bFakewalking = false;
	else
		bFakewalking = false;

	return bFakewalking;
}
