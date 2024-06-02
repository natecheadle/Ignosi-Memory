#include "dll_allocate.h"

#include <cstdlib>
#include <limits>

DLL_FUNC(void*) dll_allocate(std::uint64_t size) {
  if (size > std::numeric_limits<size_t>::max()) {
    return nullptr;
  }
  return std::malloc(static_cast<size_t>(size));
}

DLL_FUNC(void) dll_deallocate(void* obj) { return std::free(obj); }
