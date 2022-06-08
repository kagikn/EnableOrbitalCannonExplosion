#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <vector>
#include <sstream>

HMODULE _dllModule;

namespace {
    template<typename Out>
    void split(const std::string& s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> elems;
        ::split(s, delim, std::back_inserter(elems));
        return elems;
    }
}

uintptr_t FindPattern(const char* patternStr) {
    std::vector<std::string> bytesStr = split(patternStr, ' ');

    MODULEINFO moduleInfo{};
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));

    auto* start_offset = static_cast<uint8_t*>(moduleInfo.lpBaseOfDll);
    const auto size = static_cast<uintptr_t>(moduleInfo.SizeOfImage);

    uintptr_t pos = 0;
    const uintptr_t searchLen = bytesStr.size();

    for (auto* retAddress = start_offset; retAddress < start_offset + size; retAddress++) {
        if (bytesStr[pos] == "??" || bytesStr[pos] == "?" ||
            *retAddress == static_cast<uint8_t>(std::strtoul(bytesStr[pos].c_str(), nullptr, 16))) {
            if (pos + 1 == bytesStr.size())
                return (reinterpret_cast<uintptr_t>(retAddress) - searchLen + 1);
            pos++;
        }
        else {
            pos = 0;
        }
    }
    return 0;
}

DWORD WINAPI PatchOrbitalCannonBlocker()
{
    while (true) {
        // just wait until the game assembly is deployed
        auto gameStateAddr = FindPattern("D1 E9 F6 C1 01 75 19 8B");
        if (!gameStateAddr) {
            Sleep(5000);
            continue;
        }

        auto addrForExplosionAllowedFunc = FindPattern("FF 52 ? 48 8B F8 83 FB 3B 75 ?");
        if (addrForExplosionAllowedFunc) {
            uint8_t instructionToWrite[3] = { 0xB0, 0x01, 0xC3 }; // always returns 1
            memcpy((void*)(addrForExplosionAllowedFunc - 0x17), instructionToWrite, 3);
        }

        FreeLibraryAndExitThread(_dllModule, 0);
    }
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        _dllModule = hModule;
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PatchOrbitalCannonBlocker, hModule, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
