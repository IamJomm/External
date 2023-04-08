#include <thread>
#include <conio.h>
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
        if(rpm<int>(clientMod + dwEntityList) != 0){
            locPlayer = rpm<int>(clientMod + dwLocalPlayer);
            locFlags = rpm<int>(locPlayer + m_fFlags);
            if(locFlags == 257 && GetAsyncKeyState(VK_SPACE) || locFlags == 263 && GetAsyncKeyState(VK_SPACE)){
                wpm<bool>(clientMod + dwForceJump, true);
                Sleep(50);
                wpm<bool>(clientMod + dwForceJump, false);
            }
        } else Sleep(200);
    }
}

void radar(){
    int entity;
	while(true){
        if(rpm<int>(clientMod + dwEntityList) != 0){
            for(short int i = 1; i <= 64; i++){
                entity = rpm<int>(clientMod + dwEntityList + i * 0x10);
                if(entity + m_bDormant != 1)
                    wpm<bool>(entity + m_bSpotted, true);
            }
            Sleep(20);
        } else Sleep(200);
    }
}

void glow(){
    int locPlayer, glowObjectManager, entity, glowIndex;
    while(true){
        if(rpm<int>(clientMod + dwEntityList) != 0){
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
        } else Sleep(200);
    }
}

int main(){
    string names[] = {"Bhop", "Radar hack", "Glow ESP"};
    bool arr[] = {false, false, false};
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
            cout << "<" << (arr[i - 1] == true? "true": "false") << ">" << endl;
        }
        input = _getch();
        switch(input){
            case 72: if (choice > 1)
                choice--; break;
            case 80: if(choice < sizeof(names) / sizeof(names[0]))
                choice++; break;
            case 75:
            case 77:
                if (arr[choice - 1] == false)
                    arr[choice - 1] = true;
                else arr[choice - 1] = false;
                break;
        }
    }
    system("cls");
	getProcess(hProc, procId, "csgo.exe");
	clientMod = getModule(procId, "client.dll");
	cout << hProc << " " << procId << " " << "0x" << hex << clientMod << dec << endl;
    void (*functions[3])() = {bhop, radar, glow};
    thread threads[sizeof(functions)/sizeof(functions[0])];
    for(int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
        if(arr[i] == true)
            threads[i] = thread(functions[i]);
    for(int i = 0; i < sizeof(threads) / sizeof(threads[0]); i++)
        threads[i].join();
}