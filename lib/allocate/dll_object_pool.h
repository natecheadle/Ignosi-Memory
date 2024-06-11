#pragma once

#include <dll_defines.h>

#include <cstdint>

using IgnosiMemoryPool = void*;

extern "C" {
EXPORT_SPEC IgnosiMemoryPool IgnosiMemoryPoolCreate(size_t objectSize,
                                                    size_t poolSize);
EXPORT_SPEC void IgnosiMemoryPoolDestroy(IgnosiMemoryPool obj);

EXPORT_SPEC void* IgnosiMemoryPoolAllocate(IgnosiMemoryPool pool);
EXPORT_SPEC void IgnosiMemoryPoolDeallocate(IgnosiMemoryPool pool, void* pObj);
}
