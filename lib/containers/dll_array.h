#pragma once

#include <dll_defines.h>

#include <cstdint>

struct dll_array {
  void* pData;
  std::uint64_t Size;
};

DLL_FUNC(dll_array)
dll_allocate_array(std::uint64_t objectSize, std::uint64_t size);
DLL_FUNC(void) dll_deallocate_array(dll_array value);
