#pragma once
#include "aimbot.hpp"

void VectorAngles(const float* forward, float* angles)
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / 3.14f);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / 3.14f);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

Vector CalcAngle(Vector src, Vector dir, float xp, float yp)
{
	float output[3];
	float input[3] = { dir.x - src.x , dir.y - src.y, dir.z - src.z };
	VectorAngles(input, output);
	if (output[0] > 180) output[0] -= 360;
	if (output[0] < -180) output[0] += 360;
	if (output[1] > 180) output[1] -= 360;
	if (output[1] < -180) output[1] += 360;
	return { output[0] - xp * 2.f, output[1] - yp * 2.f, 0.f };
}

static bool ShouldInit = true;
void AimBot::Init(DWORD Player)
{
	// re-find target if our old target is dead, dormant, or now a teammate
	if (!Entity::Get().IsEnemy(Target, Memory::Get().Local) || !Entity::Get().IsAlive(Target) || Entity::Get().Dormant(Target))
		ShouldInit = true;

	if (!ShouldInit)
		return;

	if (!Entity::Get().IsEnemy(Player, Memory::Get().Local) || !Entity::Get().IsAlive(Player) || Entity::Get().Dormant(Player))
		return;

	Target = Player;

	ShouldInit = false;
}

// Actual AimBot::Aim function is pasted
// I'll re-write at some point
void AimBot::Aim(int SmoothAmt)
{

	Vector MyEyeLoc;
	Vector EnemyBones;
	DWORD Bonebase = Memory::Get().Read<DWORD>(Target + hazedumper::netvars::m_dwBoneMatrix);
	Vector MyLoc = Memory::Get().Read<Vector>(Memory::Get().Local + hazedumper::netvars::m_vecOrigin);
	Vector Origin = Memory::Get().Read<Vector>(Memory::Get().Local + hazedumper::netvars::m_vecViewOffset);
	int health = Memory::Get().Read<int>(Target + hazedumper::netvars::m_iHealth);
	int spottedByMask = Memory::Get().Read<int>(Target + hazedumper::netvars::m_bSpottedByMask);
	float x = Memory::Get().Read<float>(Memory::Get().ClientState + hazedumper::signatures::dwClientState_ViewAngles);
	float y = Memory::Get().Read<float>(Memory::Get().ClientState + hazedumper::signatures::dwClientState_ViewAngles + 0x4);
	MyEyeLoc.x = MyLoc.x + Origin.x;
	MyEyeLoc.y = MyLoc.y + Origin.y;
	MyEyeLoc.z = MyLoc.z + Origin.z;


	EnemyBones.x = Memory::Get().Read<float>(Bonebase + 0x30 * 8 + 0x0C);
	EnemyBones.y = Memory::Get().Read<float>(Bonebase + 0x30 * 8 + 0x1C);
	EnemyBones.z = Memory::Get().Read<float>(Bonebase + 0x30 * 8 + 0x2C);


	Vector Punch = Memory::Get().Read<Vector>(Memory::Get().Local + hazedumper::netvars::m_aimPunchAngle);
	Vector Target = CalcAngle(MyEyeLoc, EnemyBones, Punch.x, Punch.y);
	if (health > 0)
	{
		x = x + (Target.x - x) / SmoothAmt;
		if (y > 0 && Target.y < 0)
		{
			if (y > 150)
			{
				y = y + ((Target.y + 360) - y) / SmoothAmt;
			}
			else
			{
				y = y + (Target.y - y) / SmoothAmt;
			}
		}
		else if (y < 0 && Target.y > 0)
		{
			if (y < -150)
			{
				y = y - ((y + 360) - Target.y) / SmoothAmt;
			}
			else
			{
				y = y + (Target.y - y) / SmoothAmt;
			}
		}
		else
		{
			y = y + (Target.y - y) / SmoothAmt;
		}
		if (y > 180)
		{
			y -= 360;
		}
		else if (y < -180)
		{
			y += 360;
		}

		Memory::Get().Write<float>(Memory::Get().ClientState + hazedumper::signatures::dwClientState_ViewAngles, x);
		Memory::Get().Write<float>(Memory::Get().ClientState + hazedumper::signatures::dwClientState_ViewAngles + 0x4, y);
	}
}

void AimBot::Trigger()
{
    int crosshair_id = Memory::Get().Read<int>(Memory::Get().Local + hazedumper::netvars::m_iCrosshairId);

    if (crosshair_id <= 0 || crosshair_id > 300)
        return;

    DWORD entity = Memory::Get().Read<DWORD>(Memory::Get().Client + hazedumper::signatures::dwEntityList + ((crosshair_id - 1) * 0x10));

    if (!Entity::Get().IsEnemy(entity, Memory::Get().Local) || !Entity::Get().IsAlive(entity))
        return;

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(20);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}