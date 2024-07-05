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

  return static_cast<DllObjectPool*>(pool)->Allocate();
}

EXPORT_SPEC void IgnosiMemoryPoolDeallocate(IgnosiMemoryPool pool, void* pObj) {
  if (!pool) {
    return;
  }

  static_cast<DllObjectPool*>(pool)->Deallocate(pObj);
}

EXPORT_SPEC size_t IgnosiMemoryPoolSize(IgnosiMemoryPool pool) {
  if (!pool) {
    return 0;
  }
  return static_cast<DllObjectPool*>(pool)->PoolSize();
}

EXPORT_SPEC size_t IgnosiMemoryPoolAllocatedCount(IgnosiMemoryPool pool) {
  if (!pool) {
    return 0;
  }
  return static_cast<DllObjectPool*>(pool)->AllocatedCount();
}
