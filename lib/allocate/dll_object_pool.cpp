#include "dll_object_pool.h"

#include "detail/dll_object_pool_data.h"

using namespace ignosi::memory::detail;

EXPORT_SPEC IgnosiMemoryPool IgnosiMemoryPoolCreate(size_t objectSize,
                                                    size_t poolSize) {
  return new DllObjectPool(objectSize, poolSize);
}

EXPORT_SPEC void IgnosiMemoryPoolDestroy(IgnosiMemoryPool obj) {
  delete static_cast<DllObjectPool*>(obj);
}

EXPORT_SPEC void* IgnosiMemoryPoolAllocate(IgnosiMemoryPool pool) {
  if (!pool) {
    return nullptr;
  }

  return reinterpret_cast<DllObjectPool*>(pool)->Allocate();
}

EXPORT_SPEC void IgnosiMemoryPoolDeallocate(IgnosiMemoryPool pool, void* pObj) {
  if (!pool) {
    return;
  }

  reinterpret_cast<DllObjectPool*>(pool)->Dealloate(pObj);
}
