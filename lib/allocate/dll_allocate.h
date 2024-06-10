#pragma once

#include <dll_defines.h>

#include <cstdint>

extern "C" {
EXPORT_SPEC void* IgnosiMemoryAllocate(size_t size);
EXPORT_SPEC void IgnosiMemoryDeallocate(void* obj);
}
