#pragma once
#include "../includes.hpp"

class Visuals : public Singleton <Visuals>
{
public:

	void Chams(DWORD Player, bool IsEnemy);
	void Glow(DWORD Player, bool IsEnemy);
};

