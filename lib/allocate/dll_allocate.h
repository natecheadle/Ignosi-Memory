#pragma once

#include <dll_defines.h>

#include <cstdint>

extern "C" {
EXPORT_SPEC void* IgnosiMemoryAllocate(std::uint64_t size);
EXPORT_SPEC void IgnosiMemoryDeallocate(void* obj);
}
