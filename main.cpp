#include <thread>
#include <conio.h>
#include <iostream>
#include "tools.hpp"
#include "offsets.hpp"

using namespace std;

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

struct color{
    color(float r, float g, float b, float a = 1.f):
        r(r), g(g), b(b), a(a) {}; 
    float r, g, b, a;
} t = color(0.87, 0.6, 0.2, 0.8), ct = color(0.36, 0.47, 0.68, 0.8);

HANDLE hProc;
DWORD procId;
uintptr_t clientMod;
bool status[] = {false, false, false, false};

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
    while(status[0]){
        if(rpm<int>(clientMod + dwEntityList)){
            locFlags = rpm<int>(rpm<int>(clientMod + dwLocalPlayer) + m_fFlags);
            if(locFlags == 257 && GetAsyncKeyState(VK_SPACE) || locFlags == 263 && GetAsyncKeyState(VK_SPACE))
                wpm<bool>(clientMod + dwForceJump, true);
            else
                wpm<bool>(clientMod + dwForceJump, false);
            this_thread::sleep_for(chrono::milliseconds(1));
        } else this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void radar(){
    int entity;
	while(status[1]){
        if(rpm<int>(clientMod + dwEntityList)){
            for(short int i = 1; i <= 64; i++){
                entity = rpm<int>(clientMod + dwEntityList + i * 0x10);
                if(entity + m_bDormant != 1)
                    wpm<bool>(entity + m_bSpotted, true);
            }
            this_thread::sleep_for(chrono::milliseconds(10));
        } else this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void glow(){
    int locPlayer, glowObjectManager, entity, glowIndex;
    while(status[2]){
        if(rpm<int>(clientMod + dwEntityList)){
            locPlayer = rpm<int>(clientMod + dwLocalPlayer);
            glowObjectManager = rpm<int>(clientMod + dwGlowObjectManager);
            for(int i = 1; i <= 64; i++){
                entity = rpm<int>(clientMod + dwEntityList + i * 0x10);
                if(entity + m_bDormant != 1){
                    glowIndex = rpm<int>(entity + m_iGlowIndex);
                    if(rpm<int>(entity + m_iTeamNum) == 2){
                        wpm<color>(glowObjectManager + glowIndex * 0x38 + 0x8, t);
                    }
                    else{
                        wpm<color>(glowObjectManager + glowIndex * 0x38 + 0x8, ct);
                    }
                    wpm<bool>(glowObjectManager + glowIndex * 0x38 + 0x27, true);
                    wpm<bool>(glowObjectManager + glowIndex * 0x38 + 0x28, true);
                }
            }
            this_thread::sleep_for(chrono::milliseconds(15));
        } else this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void triggerbot(){
    int locPlayer, crosshairId, player;
    while(status[3]){
        if(rpm<int>(clientMod + dwEntityList)){
            this_thread::sleep_for(chrono::milliseconds(1));
            locPlayer = rpm<int>(clientMod + dwLocalPlayer);
            if(!rpm<int>(locPlayer + m_iHealth)) continue;
            crosshairId = rpm<int>(locPlayer + m_iCrosshairId);
            if(!crosshairId || crosshairId > 64) continue;
            player = rpm<int>(clientMod + dwEntityList + (crosshairId - 1) * 0x10);
            if(!rpm<int>(player + m_iHealth) || rpm<int>(locPlayer + m_iTeamNum) == rpm<int>(player + m_iTeamNum)) continue;
            wpm<int>(clientMod + dwForceAttack, 6);
            this_thread::sleep_for(chrono::milliseconds(30));
            wpm<int>(clientMod + dwForceAttack, 4);
        } else this_thread::sleep_for(chrono::milliseconds(200));
    }
}

int main(){
	getProcess(hProc, procId, "csgo.exe");
	clientMod = getModule(procId, "client.dll");
    string names[] = {"Bhop", "Radar hack", "Glow ESP", "Triggerbot"};
    void (*functions[4])() = {bhop, radar, glow, triggerbot};
    int choice = 1, max = 0;
    char input;
    for(int i = 0; i < sizeof(names) / sizeof(names[0]); i++)
        if(names[i].size() > max) max = names[i].size();
    while(input != '\r'){
        system("cls");
        for(int i = 1; i <= sizeof(names) / sizeof(names[0]); i++){
            cout << (choice == i? "> " : "  ") << names[i - 1];
            for(int j = 0; j < max - names[i - 1].size() + 1; j++)
                cout << " ";
            cout << "<" << (status[i - 1] == true? "true": "false") << ">" << endl;
        }
        input = _getch();
        switch(input){
            case 72: if (choice > 1)
                choice--; break;
            case 80: if(choice < sizeof(names) / sizeof(names[0]))
                choice++; break;
            case 75:
            case 77:
                if (status[choice - 1] == false){
                    status[choice - 1] = true;
                    thread(functions[choice - 1]).detach();
                }
                else status[choice - 1] = false;
                break;
        }
    }
}