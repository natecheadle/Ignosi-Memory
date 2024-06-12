#pragma once

#include <dll_object_pool.h>

#include <utility>

namespace ignosi::memory {

template <typename T>
class DllObjectPool {
  IgnosiMemoryPool m_pPool;

 public:
  DllObjectPool(size_t poolSize)
      : m_pPool(IgnosiMemoryPoolCreate(sizeof(T), poolSize)) {}

  ~DllObjectPool() { IgnosiMemoryPoolDestroy(m_pPool); }

  T* Create(T&& obj) {
    void* pNew = IgnosiMemoryPoolAllocate(m_pPool);
    if (!pNew) {
      return nullptr;
    }

    try {
      return new (pNew) T(std::forward<T>(obj));
    } catch (...) {
      IgnosiMemoryPoolDeallocate(m_pPool, pNew);
    }
    return nullptr;
  }

  void Destroy(T* obj) {
    obj->~T();
    IgnosiMemoryPoolDeallocate(m_pPool, obj);
  }
};
}  // namespace ignosi::memory
