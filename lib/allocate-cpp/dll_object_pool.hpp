#pragma once

#include <dll_object_pool.h>

#include <limits>
#include <new>

namespace ignosi::memory {

template <typename T>
class DllObjectPool {
  IgnosiMemoryPool m_pPool;

 public:
  DllObjectPool(size_t poolSize)
      : m_pPool(IgnosiMemoryPoolCreate(sizeof(T), poolSize)) {}

  ~DllObjectPool() { IgnosiMemoryPoolDestroy(m_pPool); }
};
}  // namespace ignosi::memory
