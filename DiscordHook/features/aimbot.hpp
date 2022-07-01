#pragma once
#include "../includes.hpp"

class AimBot : public Singleton <AimBot>
{
public:
	void Trigger();
	void Aim(int SmoothAmt);
	void Init(DWORD Player);

	DWORD Target;
};
