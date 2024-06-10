#pragma once

#include <cstdint>
#include <dll_allocate.h>

namespace ignosi::memory::detail {

struct BoostPoolAllocator {
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  static char *malloc(const size_type bytes) {
    return reinterpret_cast<char *>(IgnosiMemoryAllocate(bytes));
  }
  static void free(char *const block) { IgnosiMemoryDeallocate(block); }
};

}  // namespace ignosi::memory::detail
