#pragma once
#include "visuals.hpp"

void Visuals::Chams(DWORD Player, bool IsEnemy)
{
	Memory::Get().Write<BYTE>(Player + hazedumper::netvars::m_clrRender, IsEnemy ? 255 : 0);
	Memory::Get().Write<BYTE>(Player + hazedumper::netvars::m_clrRender + 0x1, 0);
	Memory::Get().Write<BYTE>(Player + hazedumper::netvars::m_clrRender + 0x2, IsEnemy ? 0 : 255);
}

struct GlowStruct {
	BYTE base[8];
	float R;
	float G;
	float B;
	float A;
	BYTE buffer[16];
	bool RenderOccluded;
	bool RenderUnOccluded;
	bool FullBloom;
	BYTE buffer1[5];
	int Style;
};

void Visuals::Glow(DWORD Player, bool IsEnemy)
{
	int GlowIndex = Memory::Get().Read<int>(Player + hazedumper::netvars::m_iGlowIndex);
	DWORD GlowObj = Memory::Get().Read<int>(Memory::Get().Client + hazedumper::signatures::dwGlowObjectManager);

	GlowStruct Glow = Memory::Get().Read<GlowStruct>(GlowObj + (GlowIndex * 0x38));

	Glow.R = IsEnemy ? 1.f : 0.f;
	Glow.G = 0.f;
	Glow.B = IsEnemy ? 0.f : 1.f;
	Glow.A = 0.9f;
	Glow.RenderOccluded = true;
	Glow.RenderUnOccluded = false;

	Memory::Get().Write<GlowStruct>(GlowObj + (GlowIndex * 0x38), Glow);
}
