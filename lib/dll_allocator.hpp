#pragma once

#include <limits>
#include <new>

#include "dll_allocate.h"

namespace ignosi::memory {

template <typename T>
class DllAllocator {
 public:
  typedef T value_type;

  DllAllocator() = default;

  template <class U>
  constexpr DllAllocator(const DllAllocator<U>&) noexcept {}

  [[nodiscard]] T* allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    if (auto p = static_cast<T*>(IgnosiMemoryAllocate(n * sizeof(T)))) {
      return p;
    }

    throw std::bad_alloc();
  }
  template <class... Args>
  constexpr T* construct_at(T* p, Args&&... args) {
    return new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
  }

  void deallocate(T* p, std::size_t) noexcept { IgnosiMemoryDeallocate(p); }
};

template <class T, class U>
bool operator==(const DllAllocator<T>&, const DllAllocator<U>&) {
  return true;
}

template <class T, class U>
bool operator!=(const DllAllocator<T>&, const DllAllocator<U>&) {
  return false;
}
}  // namespace ignosi::memory
