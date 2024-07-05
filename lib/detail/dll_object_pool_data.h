#pragma once

#include <array>
#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <memory>
#include <mutex>

namespace ignosi::memory::detail {

class DllObjectPool {
  std::atomic<size_t> m_AllocatedCount{0};
  boost::lockfree::stack<void*> m_FreeObjects;

  const size_t m_ObjectSize{0};
  const size_t m_PoolSize{0};

  std::array<std::unique_ptr<std::uint8_t[]>, 256> m_Buffers;
  std::atomic<size_t> m_BuffersSize{0};

 public:
  DllObjectPool(size_t objectSize, size_t poolSize);
  ~DllObjectPool();

  DllObjectPool(const DllObjectPool& other) = delete;
  DllObjectPool(DllObjectPool&& other) noexcept = delete;

  DllObjectPool& operator=(const DllObjectPool& other) = delete;
  DllObjectPool& operator=(DllObjectPool&& other) noexcept = delete;

  void* Allocate();
  void Deallocate(void* pObj);

  size_t PoolSize() const;
  size_t AllocatedCount() const;
  size_t MaxAllocatedCount() const;

 private:
  void initializeNewBufferBlock();
};

}  // namespace ignosi::memory::detail
