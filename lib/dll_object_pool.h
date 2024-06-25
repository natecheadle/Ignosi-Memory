#pragma once

#include <cstddef>

using IgnosiMemoryPool = void*;

extern "C" {
EXPORT_SPEC IgnosiMemoryPool IgnosiMemoryPoolCreate(size_t objectSize,
                                                    size_t poolSize);
EXPORT_SPEC void IgnosiMemoryPoolDestroy(IgnosiMemoryPool obj);

EXPORT_SPEC void* IgnosiMemoryPoolAllocate(IgnosiMemoryPool pool);
EXPORT_SPEC void IgnosiMemoryPoolDeallocate(IgnosiMemoryPool pool, void* pObj);
EXPORT_SPEC size_t IgnosiMemoryPoolSize(IgnosiMemoryPool pool);
EXPORT_SPEC size_t IgnosiMemoryPoolAllocatedCount(IgnosiMemoryPool pool);
}
