#include "dll_object_pool.h"
#include "detail/dll_object_pool_wrapper.h"

using namespace ignosi::memory::detail;

EXPORT_SPEC IgnosiMemoryPool IgnosiMemoryPoolCreate(std::uint32_t objectSize){
  return new DllObjectPoolWrapper(objectSize);
}

EXPORT_SPEC void IgnosiMemoryPoolDestroy(IgnosiMemoryPool obj) {
  delete static_cast<DllObjectPoolWrapper*>(obj);
}
