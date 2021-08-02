#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "offsets.h"

HWND hwnd = 00000000;
DWORD procid;
HANDLE hProcess;
uintptr_t gameModule;
int local_player;
int entity_list;
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
	while (true)
	{
		if (hwnd != FindWindowA(NULL, "Counter-Strike: Global Offensive"))
		{
			local_player = 0;
			entity_list = 0;
			do
			{
				do
				{
					hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
					if (hwnd != 00000000)
					{
						GetWindowThreadProcessId(hwnd, &procid);
						hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procid);
					}
					Sleep(100);
				} while (hwnd == 00000000);
				gameModule = GetModule("client.dll");
				ReadProcessMemory(hProcess, (PVOID)(gameModule + dwEntityList), &entity_list, sizeof(entity_list), NULL);
				if (entity_list != 0)
				{
					ReadProcessMemory(hProcess, (PVOID)(gameModule + dwLocalPlayer), &local_player, sizeof(local_player), NULL);
				}
			} while (entity_list == 0);
		}

		ReadProcessMemory(hProcess, (PVOID)(gameModule + dwLocalPlayer), &local_player, sizeof(local_player), NULL);
		ReadProcessMemory(hProcess, (PVOID)(local_player + m_fFlags), &local_flags, sizeof(local_flags), NULL);

		if (local_flags == 257 && GetAsyncKeyState(VK_SPACE) || local_flags == 263 && GetAsyncKeyState(VK_SPACE))
		{
			value = true;
			WriteProcessMemory(hProcess, (PVOID)(gameModule + dwForceJump), &value, sizeof(value), NULL);
			Sleep(20);
			value = false;
			WriteProcessMemory(hProcess, (PVOID)(gameModule + dwForceJump), &value, sizeof(value), NULL);
		}
	}
}