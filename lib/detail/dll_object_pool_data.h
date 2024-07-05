#pragma once

#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <memory>
#include <mutex>

namespace ignosi::memory::detail {

class DllObjectPool {
  const size_t m_ObjectSize;
  const size_t m_PoolSize;

  boost::lockfree::stack<void*> m_FreeObjects;
  boost::lockfree::stack<std::shared_ptr<std::uint8_t[]>> m_Buffers;

  std::atomic<size_t> m_AllocatedCount{0};
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

 private:
  void initializeNewBufferBlock();
};

}  // namespace ignosi::memory::detail
