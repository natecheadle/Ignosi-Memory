#include "dll_allocate.h"

#include <cstdlib>
#include <limits>

EXPORT_SPEC void* IgnosiMemoryAllocate(size_t size) {
  return std::malloc(size);
}

EXPORT_SPEC void IgnosiMemoryDeallocate(void* obj) { return std::free(obj); }
