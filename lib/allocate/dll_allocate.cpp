#include "dll_allocate.h"

#include <cstdlib>
#include <limits>

EXPORT_SPEC void* IgnosiMemoryAllocate(std::uint64_t size) {
  if (size > std::numeric_limits<size_t>::max()) {
    return nullptr;
  }
  return std::malloc(static_cast<size_t>(size));
}

EXPORT_SPEC void IgnosiMemoryDeallocate(void* obj) { return std::free(obj); }
