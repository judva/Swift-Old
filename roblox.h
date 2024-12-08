#pragma once
#include <Windows.h>
#define offset_to_0(x)  x  + (uintptr_t)GetModuleHandle(NULL)
namespace offsets {
	const uintptr_t jobs_start_offset = 0x198;
	const uintptr_t jobs_end_offset = 0x1A0;
	const uintptr_t jobs_name_offset = 0x90;

	const uintptr_t Capabilities = 0x3FFFFF00;
	const uintptr_t ScriptContextJob = 0x1F8;
	const uintptr_t ScriptContextName = 0x48;
	const uintptr_t ScriptContextState = 0x110;

	const uintptr_t DecryptState = 0x88;



}

namespace addresses {
	const uintptr_t print = offset_to_0(0x11510F0);
	const uintptr_t GetTaskScheduler = offset_to_0(0x2BAC820);
	const uintptr_t LuaVm_Load = offset_to_0(0xA91C50);
	const uintptr_t LuaGetState = offset_to_0(0xCDE2C0);
	const uintptr_t LuaDecryptState = offset_to_0(0xA8F690);
	const uintptr_t TaskDefer = offset_to_0(0xE4A780);
}