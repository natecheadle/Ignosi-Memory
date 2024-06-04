#pragma once

#include <dll_allocate.h>

#include <limits>
#include <new>

namespace ignosi::memory {

template <typename T>
class dll_allocator {
 public:
  typedef T value_type;

  dll_allocator() = default;

  template <class U>
  constexpr dll_allocator(const dll_allocator<U>&) noexcept {}

  [[nodiscard]] T* allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    if (auto p = static_cast<T*>(ignosi_memory_allocate(n * sizeof(T)))) {
      return p;
    }

    throw std::bad_alloc();
  }

  void deallocate(T* p, std::size_t) noexcept { ignosi_memory_deallocate(p); }
};

template <class T, class U>
bool operator==(const dll_allocator<T>&, const dll_allocator<U>&) {
  return true;
}

template <class T, class U>
bool operator!=(const dll_allocator<T>&, const dll_allocator<U>&) {
  return false;
}
}  // namespace Ignosi::dll
