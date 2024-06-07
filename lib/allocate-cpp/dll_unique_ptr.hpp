#pragma once

#include <dll_allocate.h>

#include <cassert>
#include <memory>
#include <utility>

namespace ignosi::memory {

template <typename T>
using DllUniquePtr = std::unique_ptr<T, void (*)(T*)>;

template <typename T>
static void Destroy(T* pObj) {
  if (pObj) {
    pObj->~T();
    IgnosiMemoryDeallocate(pObj);
  }
}

template <typename T, typename... Args>
DllUniquePtr<T> MakeUniqueDllObject(Args... args) {
  void* pNewMem = IgnosiMemoryAllocate(sizeof(T));

  if (pNewMem) {
    try {
      T* newObj = new (pNewMem) T(std::forward<Args>(args)...);
      return DllUniquePtr<T>(newObj, &Destroy);
    } catch (...) {
      IgnosiMemoryDeallocate(pNewMem);
      throw;
    }
  }
  throw std::bad_alloc();
}

template <typename U, typename T>
DllUniquePtr<U> CastDllUniquePtr(DllUniquePtr<T>&& obj) {
  return DllUniquePtr<U>(static_cast<U*>(obj.release()), &Destroy<U>);
}

}  // namespace ignosi::memory
