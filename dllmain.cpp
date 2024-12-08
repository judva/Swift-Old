#include <Windows.h>
#include<iostream>
#include<format>
#include <tlhelp32.h>
#include <string>
#include <thread>
#include "execution/execution.hpp"



int EXEC_THINGY = 0;

#define offset_to_0(x)  x  + (uintptr_t)GetModuleHandle(NULL)

const uintptr_t jobs_start_offset = 0x198;
const uintptr_t jobs_end_offset = 0x1A0;
const uintptr_t jobs_name_offset = 0x90;

void main_thread() {
    uintptr_t EncryptedState;


    uintptr_t DataModel = 0x0;


    //Print Stuff
    typedef int(__fastcall* pPrint)(unsigned int, const char*, ...);
    pPrint PrintFunction = (pPrint)(offset_to_0(0x11510F0));
    PrintFunction(1, "Swift is now injected in roblox !");

    execution::execute("game:GetService('Players').LocalPlayer.Character.Humanoid.WalkSpeed = 50");
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread(main_thread).detach();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int n(int code, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(NULL, code, wParam, lParam); }