#include <thread>
#include "tools.hpp"
#include "offsets.hpp"

using namespace std;

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

HANDLE hProc;
DWORD procId;
uintptr_t clientMod;

template <class datatype>
void wpm(DWORD address, datatype value){
	WriteProcessMemory(hProc, (PVOID)address, &value, sizeof(datatype), 0);
}
template <class datatype>
datatype rpm(DWORD address){
	datatype rpmBuffer;
	ReadProcessMemory(hProc, (PVOID)address, &rpmBuffer, sizeof(datatype), 0);
	return rpmBuffer;
}

void bhop(){
    int locPlayer, locFlags;
    while(true){
        locPlayer = rpm<int>(clientMod + dwLocalPlayer);
        locFlags = rpm<int>(locPlayer + m_fFlags);
        if(locFlags == 257 && GetAsyncKeyState(VK_SPACE) || locFlags == 263 && GetAsyncKeyState(VK_SPACE)){
            wpm<bool>(clientMod + dwForceJump, true);
            Sleep(50);
            wpm<bool>(clientMod + dwForceJump, true);
        }
    }
}

void radar(){
    int locPlayer, entity;
	while(true){
        locPlayer = rpm<int>(clientMod + dwLocalPlayer);
        for(short int i = 1; i <= 64; i++){
            entity = rpm<int>(clientMod + dwEntityList + i * 0x10);
            if(entity + m_bDormant != 1)
                wpm<bool>(entity + m_bSpotted, true);
        }
        Sleep(20);
    }
}

void glow(){
    int locPlayer, glowObjectManager, entity, glowIndex;
    while(true){
        locPlayer = rpm<int>(clientMod + dwLocalPlayer);
        glowObjectManager = rpm<int>(clientMod + dwGlowObjectManager);
        for(int i = 1; i <= 64; i++){
            entity = rpm<int>(clientMod + dwEntityList + i * 0x10);
            if(entity + m_bDormant != 1){
                glowIndex = rpm<int>(entity + m_iGlowIndex);
                if(rpm<int>(entity + m_iTeamNum) == 2){
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x8, 0.87f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0xC, 0.6f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x10, 0.2f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x14, 0.8f);
                }
                else{
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x8, 0.36f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0xC, 0.47f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x10, 0.68f);
                    wpm<float>(glowObjectManager + glowIndex * 0x38 + 0x14, 0.8f);
                }
                wpm<bool>(glowObjectManager + glowIndex * 0x38 + 0x27, true);
                wpm<bool>(glowObjectManager + glowIndex * 0x38 + 0x28, true);
            }
        }
        Sleep(15);
    }
}

int main(){
	getProcess(hProc, procId, "csgo.exe");
	clientMod = getModule(procId, "client.dll");
	cout << hProc << " " << procId << " " << "0x" << hex << clientMod << dec << endl;
	thread thBhop(bhop);
    thread thRadar(radar);
    thread thGlow(glow);
    thBhop.join();
    thRadar.join();
    thGlow.join();
}