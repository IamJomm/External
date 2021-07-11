#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "offsets.h"

#define FL_ON_GROUND 257

DWORD procid;
HANDLE hProcess;
uintptr_t moduleBase;
DWORD gameModule;
DWORD local_player;
int local_flags;
bool value;

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

auto GetModule(const char* modName) -> DWORD
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procid);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnap, &modEntry))
		{
			while (Module32Next(hSnap, &modEntry))
			{
				if (!strcmp(modEntry.szModule, modName))
				{
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			}
		}
	}
}

int main()
{
	GetWindowThreadProcessId(FindWindowA(NULL, "Counter-Strike: Global Offensive"), &procid);
	moduleBase = GetModule("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procid);
	while (true)
	{
		ReadProcessMemory(hProcess, (PVOID)(moduleBase + dwLocalPlayer), &local_player, sizeof(local_player), NULL);
		ReadProcessMemory(hProcess, (PVOID)(local_player + m_fFlags), &local_flags, sizeof(local_flags), NULL);

		if (local_flags == FL_ON_GROUND && GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			value = true;
			WriteProcessMemory(hProcess, (PVOID)(moduleBase + dwForceJump), &value, sizeof(value), NULL);
			Sleep(20);
			value = false;
			WriteProcessMemory(hProcess, (PVOID)(moduleBase + dwForceJump), &value, sizeof(value), NULL);
		}
	}
}