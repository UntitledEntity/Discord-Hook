#pragma once
#include "../includes.hpp"
#include "offsets/offsets.hpp"

class Memory : public Singleton <Memory>
{
public:
	HANDLE Handle;
	INT32  ProcId;
	DWORD  Local;
	DWORD  Client, Engine;
	DWORD  ClientState;

	bool Init();

	template <class val>
	val Read(DWORD addr)
	{
		val x;
		ReadProcessMemory(Handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}

	template <class val>
	val Write(DWORD addr, val x)
	{
		WriteProcessMemory(Handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	
	DWORD GetProcess(const char* name);
	uintptr_t GetModule(DWORD procId, const char* name);
};

class Entity : public Singleton <Entity>
{
public:

	bool Dormant(DWORD Entity)
	{
		return Memory::Get().Read<bool>(Entity + hazedumper::signatures::m_bDormant);
	}

	int Team(DWORD Entity)
	{
		return Memory::Get().Read<int>(Entity + hazedumper::netvars::m_iTeamNum);
	}

	bool IsEnemy(DWORD Entity, DWORD From)
	{
		return (bool)(Memory::Get().Read<int>(Entity + hazedumper::netvars::m_iTeamNum) != Memory::Get().Read<int>(From + hazedumper::netvars::m_iTeamNum));
	}

	int Health(DWORD Entity)
	{
		return Memory::Get().Read<int>(Entity + hazedumper::netvars::m_iHealth);
	}

	bool IsAlive(DWORD Entity)
	{
		return Memory::Get().Read<int>(Entity + hazedumper::netvars::m_iHealth) > 0;
	}

	int GetDistance(DWORD Entity, DWORD From)
	{
		Vector enemy_pos = Memory::Get().Read<Vector>(Entity + hazedumper::netvars::m_vecOrigin);
		Vector my_pos = Memory::Get().Read<Vector>(Memory::Get().Local + hazedumper::netvars::m_vecOrigin);

		my_pos.x = my_pos.x - enemy_pos.x;
		my_pos.y = my_pos.y - enemy_pos.y;
		my_pos.z = my_pos.z - enemy_pos.z;

		return (int)sqrt(my_pos.x * my_pos.x + my_pos.y * my_pos.y + my_pos.z + my_pos.z) / 10;
	}
};
