#pragma once

#include <utility>

#include "dll_object_pool.h"
#include "dll_unique_ptr.hpp"

namespace ignosi::memory {

template <typename T>
class DllObjectPool {
  IgnosiMemoryPool m_pPool;

 public:
  DllObjectPool(size_t poolSize)
      : m_pPool(IgnosiMemoryPoolCreate(sizeof(T), poolSize)) {}

  ~DllObjectPool() { IgnosiMemoryPoolDestroy(m_pPool); }

  DllUniquePtr<T> Create(T&& obj) {
    void* pNew = IgnosiMemoryPoolAllocate(m_pPool);
    if (!pNew) {
      return nullptr;
    }

    try {
      return {new (pNew) T(std::forward<T>(obj)),
              [this](T* obj) { Destroy(obj); }};
    } catch (...) {
      IgnosiMemoryPoolDeallocate(m_pPool, pNew);
    }
    return nullptr;
  }

  DllUniquePtr<T> Create(const T& obj) { return Create(T(obj)); }

  void Destroy(T* obj) {
    obj->~T();
    IgnosiMemoryPoolDeallocate(m_pPool, obj);
  }

  size_t PoolSize() const { return IgnosiMemoryPoolSize(m_pPool); }
  size_t AllocatedCount() const {
    return IgnosiMemoryPoolAllocatedCount(m_pPool);
  }
  size_t MaxAllocatedCount() const {
    return IgnosiMemoryPoolMaxAllocatedCount(m_pPool);
  }
};

}  // namespace ignosi::memory
