#pragma once

#include <dll_allocate.h>
#include <dll_defines.h>

#include <cstdint>
#include <dll_allocator.hpp>

struct dll_pool_bucket {
  void* pObjects;
  std::uint64_t ObjectCount;
  std::uint64_t ObjectSize;
};

DLL_FUNC(dll_pool_bucket)
create_dll_pool_bucket(std::uint64_t objectCount, std::uint64_t objectSize);
DLL_FUNC(void) destroy_dll_pool_bucket(dll_pool_bucket value);
