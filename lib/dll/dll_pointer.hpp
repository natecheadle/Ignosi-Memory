#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include "dll_allocate.h"

namespace Ignosi::dll {

template <typename T>
using dll_unique_ptr = std::unique_ptr<T, void (*)(T*)>;

template <typename T>
static void destroy(T* pObj) {
  if (pObj) {
    pObj->~T();
    dll_deallocate(pObj);
  }
}

template <typename T, typename... Args>
dll_unique_ptr<T> make_unique_dll_obj(Args... args) {
  void* pNewMem = dll_allocate(sizeof(T));

  if (pNewMem) {
    try {
      T* newObj = new (pNewMem) T(std::forward<Args>(args)...);
      return dll_unique_ptr<T>(newObj, &destroy);
    } catch (...) {
      dll_deallocate(pNewMem);
      throw;
    }
  }
  throw std::bad_alloc();
}

template <typename U, typename T>
dll_unique_ptr<U> cast_unique_dll_ptr(dll_unique_ptr<T>&& obj) {
  return dll_unique_ptr<U>(static_cast<U*>(obj.release()), &destroy<U>);
}

}  // namespace Ignosi::dll
