#pragma once

#include <dll_defines.h>

#include <cstdint>

using IgnosiMemoryPool = void*;

extern "C" {
EXPORT_SPEC IgnosiMemoryPool IgnosiMemoryPoolCreate(std::uint32_t objectSize);
EXPORT_SPEC void IgnosiMemoryPoolDestroy(IgnosiMemoryPool obj);
}
