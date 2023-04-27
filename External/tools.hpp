#include <windows.h>
#include <tlhelp32.h>

void getProcess(HANDLE &hProc, DWORD &procId, const char* procName) {
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE ProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	while (Process32Next(ProcSnap, &procEntry))
		if (!strcmp(procName, procEntry.szExeFile)) {
			hProc = OpenProcess(PROCESS_ALL_ACCESS, false, procEntry.th32ProcessID);
			procId = procEntry.th32ProcessID;
			break;
		}
	CloseHandle(ProcSnap);
}

uintptr_t getModule(DWORD procId, const char* modName) {
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);
	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	while (Module32Next(modSnap, &modEntry))
		if (!strcmp(modName, modEntry.szModule)) {
			CloseHandle(modSnap);
			return (uintptr_t)modEntry.modBaseAddr;
		}
}