
#include "mem.hpp"
#include "../includes.hpp"

DWORD Memory::GetProcess(const char* name)
{
	HANDLE ProcIdHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD Proc = NULL;
	PROCESSENTRY32 Entry{};
	Entry.dwSize = sizeof(Entry);

	while (Process32Next(ProcIdHandle, &Entry))
	{
		if (!strcmp((const char*)Entry.szExeFile, name))
		{
			Proc = Entry.th32ProcessID;
			CloseHandle(ProcIdHandle);
			/*Handle = HijackHandle::Get().HijakExistingHandle(Proc);*/
			Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Proc);
		}
	}

	return Proc;
}

uintptr_t Memory::GetModule(DWORD procId, const char* modName) {

	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	do
	{
		if (!strcmp(mEntry.szModule, modName))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.hModule;
		}
	} while (Module32Next(hModule, &mEntry));
	return 0;
}

bool Memory::Init() {
	Handle = nullptr;

	ProcId = GetProcess("csgo.exe");

	Client = GetModule(ProcId, "client.dll");
	Engine = GetModule(ProcId, "engine.dll");

	ClientState = Read<DWORD>(Engine + hazedumper::signatures::dwClientState);

	return true;
}