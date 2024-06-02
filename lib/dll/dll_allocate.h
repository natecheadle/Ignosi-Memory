#pragma once

#include <cstdint>

#include "dll_defines.h"

DLL_FUNC(void*) dll_allocate(std::uint64_t size);
DLL_FUNC(void) dll_deallocate(void* obj);
