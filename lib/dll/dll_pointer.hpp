#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include "dll_allocate.h"

namespace Ignosi::dll {

template <typename T>
using dll_unique_ptr = std::unique_ptr<T, void (*)(T*)>;

template <typename T>
class dll_pointer {
  T* m_pObj{nullptr};

 public:
  dll_pointer() = default;
  dll_pointer(T* pObj) : m_pObj(pObj) {}

  dll_pointer(std::unique_ptr<T>&& other) : m_pObj(other.release()) {}

  dll_pointer(const dll_pointer& other) = delete;
  dll_pointer(dll_pointer&& other) noexcept : m_pObj(other.release()) {}

  template <typename U>
  dll_pointer(dll_pointer<U>&& other) noexcept
      : m_pObj(static_cast<T*>(other.release())) {}

  ~dll_pointer() {
    destroy(m_pObj);
    m_pObj = nullptr;
  }

  T* operator->() noexcept { return m_pObj; }
  const T* operator->() const noexcept { return m_pObj; }

  T& operator*() {
    assert(m_pObj);
    return *m_pObj;
  }
  const T& operator*() const {
    assert(m_pObj);
    return *m_pObj;
  }

  dll_pointer& operator=(const dll_pointer& other) = delete;
  dll_pointer& operator=(dll_pointer&& other) noexcept {
    reset(other.release());
    return *this;
  }

  template <typename U>
  dll_pointer& operator=(dll_pointer<U>&& other) noexcept {
    reset(static_cast<T*>(other.release()));
    return *this;
  }

  T* get() noexcept { return m_pObj; }
  const T* get() const noexcept { return m_pObj; }

  T* release() {
    T* pTmp = m_pObj;
    m_pObj = nullptr;
    return pTmp;
  }

  void reset(T* pObj) noexcept {
    destroy(m_pObj);
    m_pObj = pObj;
  }

  friend bool operator==(const dll_pointer& lhs,
                         const dll_pointer& rhs) = default;

  friend bool operator!=(const dll_pointer& lhs,
                         const dll_pointer& rhs) = default;

  explicit operator bool() const noexcept { return m_pObj != nullptr; }

  static void destroy(T* pObj) {
    if (pObj) {
      pObj->~T();
      dll_deallocate(pObj);
    }
  }
};

template <typename T, typename... Args>
dll_pointer<T> make_unique_dll_obj(Args... args) {
  void* pNewMem = dll_allocate(sizeof(T));

  if (pNewMem) {
    try {
      T* newObj = new (pNewMem) T(std::forward<Args>(args)...);
      return dll_pointer<T>(newObj);
    } catch (...) {
      dll_deallocate(pNewMem);
      throw;
    }
  }
  throw std::bad_alloc();
}

}  // namespace Ignosi::dll
