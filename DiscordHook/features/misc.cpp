#include "misc.hpp"

void Misc::Bhop() 
{
	BYTE flag = Memory::Get().Read<BYTE>(Memory::Get().Local + hazedumper::netvars::m_fFlags);
	if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0))
	{
		Memory::Get().Write<DWORD>(Memory::Get().Client + hazedumper::signatures::dwForceJump, 6);
	}
}