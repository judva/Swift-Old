#include <Windows.h>
#include<iostream>
#include<format>
#include <tlhelp32.h>
#include <string>
#include "../scheduler/scheduler.hpp"
#include "../roblox.h"
#include "execution.hpp"
#include "zstd.h"
#include "xxhash.h"
#include "Compiler/include/Luau/Compiler.h"
#include "Compiler/include/Luau/BytecodeBuilder.h"
#include "Common/include/Luau/BytecodeUtils.h"
#include "Common/include/Luau/Bytecode.h"
#include "VM/src/lstate.h"



class bytecode_encoder_t : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* data, size_t count) override {
        // Loop through the instructions.
        for (auto i = 0u; i < count;) {
            // Get the opcode (which is the first byte of the instruction).
            auto& opcode = *reinterpret_cast<uint8_t*>(data + i);

            // Add the instruction length (which could be multiple 32-bit integers).
            i += Luau::getOpLength(LuauOpcode(opcode));

            // Encode the opcode.
            opcode *= 227;
        }
    }
};

std::string compress_bytecode(std::string_view bytecode) {
    // Create a buffer.
    const auto data_size = bytecode.size();
    const auto max_size = ZSTD_compressBound(data_size);
    auto buffer = std::vector<char>(max_size + 8);

    // Copy RSB1 and data size into the buffer.
    strcpy_s(&buffer[0], buffer.capacity(), "RSB1");
    memcpy_s(&buffer[4], buffer.capacity(), &data_size, sizeof(data_size));

    // Copy compressed bytecode into the buffer.
    const auto compressed_size = ZSTD_compress(&buffer[8], max_size, bytecode.data(), data_size, ZSTD_maxCLevel());
    if (ZSTD_isError(compressed_size))
        throw std::runtime_error("Failed to compress the bytecode.");

    // Encrypt the buffer.
    const auto size = compressed_size + 8;
    const auto key = XXH32(buffer.data(), size, 42u);
    const auto bytes = reinterpret_cast<const uint8_t*>(&key);

    for (auto i = 0u; i < size; ++i)
        buffer[i] ^= bytes[i % 4] + i * 41u;

    // Create and return output.
    return std::string(buffer.data(), size);
}

typedef uintptr_t LuaProto;

void Elevate(std::uintptr_t Proto)
{
    // All capabilities
    std::uintptr_t Capabilities = 0x3FFFFF00 | 0x3F;

    // 0x78 = Userdata
    *reinterpret_cast<std::uintptr_t**>(Proto + 0x78) = &Capabilities;

    std::uintptr_t* Proto_Ptr = (std::uintptr_t*)((Proto + 0x10) + *reinterpret_cast<std::uintptr_t*>(Proto + 0x10));

    reinterpret_cast<int(__fastcall*)(int, const char*, ...)>(addresses::print)(0, "Evevating prto: %p", Proto);

    // Go through descendants if there are any
    for (int i = 0; i < *reinterpret_cast<int*>(Proto + 0x28); i++)
        Elevate(Proto_Ptr[i]);
}


void set_identity(int64_t UserData) {
    *reinterpret_cast<std::int64_t*>(UserData + 0x30) = 8;
    *reinterpret_cast<std::int64_t*>(UserData + 0x48) = 0x3FFFFF00 | 0x3F;
}

namespace execution {

    void execute(std::string source) {

        

        auto new_scheduler = swift::scheduler::get_singleton();

        auto lua_state = new_scheduler->get_state();
        lua_State* prased_lua_state = reinterpret_cast<lua_State*>(lua_state);

        int64_t UserData = reinterpret_cast<int64_t>(prased_lua_state->userdata);

        set_identity(UserData);

        static auto encoder = bytecode_encoder_t();
        const auto bytecode = Luau::compile(source, {}, {}, &encoder);
        auto compressed = compress_bytecode(bytecode);

        //luavm_load
        if (reinterpret_cast<int(__fastcall*)(uintptr_t, std::string*, const char*, OPTIONAL int)>(addresses::LuaVm_Load)(lua_state, &compressed, "&Swift", 0)) {
            reinterpret_cast<int(__fastcall*)(int, const char*, ...)>(addresses::print)(0, "Error while runing luavm::load");
        }

       
        auto ptr = reinterpret_cast<uintptr_t>(lua_topointer(prased_lua_state, 0));
        Elevate((*reinterpret_cast<uintptr_t*>(ptr + 0x18) + (ptr + 0x18)));


        reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(addresses::TaskDefer)(lua_state);
        while (true) {};

    }

}

