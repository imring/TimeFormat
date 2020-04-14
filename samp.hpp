#pragma once
#include <windows.h>
#include <stdint.h>

enum SampVersion {
    SAMP_UNKNOWN = -1,

    SAMP_0_3_7_R1 = 0,
    SAMP_0_3_7_R3_1,
    SAMP_0_3_7_R4
};

uint32_t samp_addr(uint32_t addr = 0) {
    static uint32_t samp_base = reinterpret_cast<uint32_t>(GetModuleHandle("samp.dll"));
    return samp_base + addr;
}

// thx fyp <3
SampVersion DetermineSampVersion() {
    auto base = samp_addr();
    auto ntheader = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
    auto ep = ntheader->OptionalHeader.AddressOfEntryPoint;
    switch (ep) {
    case 0x31DF13: return SampVersion::SAMP_0_3_7_R1;
    case 0xCC4D0:  return SampVersion::SAMP_0_3_7_R3_1;
    case 0xCBCB0:  return SampVersion::SAMP_0_3_7_R4;
    }
    return SampVersion::SAMP_UNKNOWN;
}

uintptr_t strftime_addr[] = {
    0xB87E7, // R1
    0xCA970, // R3-1
    0xCA150, // R4
};

uintptr_t spaces_addr[][2] = {
    { 0x63EAF + 0x8, 0x63F10 + 0x8 }, // R1
    { 0x672FF + 0x8, 0x67360 + 0x8 }, // R3-1
    { 0x67A3F + 0x8, 0x67AA0 + 0x8 }, // R4
};