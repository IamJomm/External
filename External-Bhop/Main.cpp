#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "offsets.h"

HWND hwnd = 0;
DWORD procid;
HANDLE hProcess;
uintptr_t clientModule;
int entityList;
int localPlayer;
int localFlags;
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
		if (hwnd != FindWindowA(NULL, "Counter-Strike: Global Offensive") || hwnd == 0)
		{
			hwnd = 0;
			localPlayer = 0;
			entityList = 0;
			while (localPlayer == 0)
			{
				do
				{
					hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
					if (hwnd != 0)
					{
						GetWindowThreadProcessId(hwnd, &procid);
						hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procid);
					}
					else Sleep(100);
				} while (hwnd == 0);
				while (localPlayer == 0 && hwnd == FindWindowA(NULL, "Counter-Strike: Global Offensive"))
				{
					clientModule = GetModule("client.dll");
					ReadProcessMemory(hProcess, (PVOID)(clientModule + dwEntityList), &entityList, sizeof(entityList), NULL);
					if (entityList != 0) ReadProcessMemory(hProcess, (PVOID)(clientModule + dwLocalPlayer), &localPlayer, sizeof(localPlayer), NULL);
					else Sleep(100);
				}
			}
		}

		ReadProcessMemory(hProcess, (PVOID)(clientModule + dwEntityList), &entityList, sizeof(entityList), NULL);
		if (entityList != 0)
		{
			ReadProcessMemory(hProcess, (PVOID)(clientModule + dwLocalPlayer), &localPlayer, sizeof(localPlayer), NULL);
			ReadProcessMemory(hProcess, (PVOID)(localPlayer + m_fFlags), &localFlags, sizeof(localFlags), NULL);

			if (localFlags == 257 && GetAsyncKeyState(VK_SPACE) || localFlags == 263 && GetAsyncKeyState(VK_SPACE))
			{
				value = true;
				WriteProcessMemory(hProcess, (PVOID)(clientModule + dwForceJump), &value, sizeof(value), NULL);
				Sleep(25);
				value = false;
				WriteProcessMemory(hProcess, (PVOID)(clientModule + dwForceJump), &value, sizeof(value), NULL);
			}
		}
		else Sleep(100);
	}
}