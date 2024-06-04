#pragma once

#include <dll_defines.h>

#include <cstdint>

extern "C" {
EXPORT_SPEC void* ignosi_memory_allocate(std::uint64_t size);
EXPORT_SPEC void ignosi_memory_deallocate(void* obj);
}
