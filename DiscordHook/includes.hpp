#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>

#include "singleton.hpp"
#include "vector.hpp"

#include "memory/mem.hpp"
#include "memory/offsets/offsets.hpp"

#include "features/misc.hpp"
#include "features/aimbot.hpp"
#include "features/visuals.hpp"

namespace Utils
{
	inline void OpenConsole()
	{
		AllocConsole();

		FILE* file;
		freopen_s(&file, "CONOUT$", "w", stdout);
	}

	inline void CloseConsole()
	{
		fclose(static_cast<_iobuf*>(stdout));

		FreeConsole();
	}

	inline void Print(const char* text, bool debug = false)
	{
#if !_DEBUG
		if (debug)
			return;
#else
		if (debug)
			printf("[DEBUG] ");
#endif
		printf(text);
		printf("\n");
	}

	inline DWORD_PTR GetPid(LPCSTR name) {

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, false);
		if (hSnap && hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 procEntry;
			ZeroMemory(procEntry.szExeFile, sizeof(procEntry.szExeFile));

			do
			{
				if (lstrcmpi(procEntry.szExeFile, name) == NULL) {
					return procEntry.th32ProcessID;
					CloseHandle(hSnap);
				}
			} while (Process32Next(hSnap, &procEntry));
		}
		
		return NULL;
	}
}

struct Vector4
{
	float x, y, z, w;
};

struct view
{
	float v[16];
};
