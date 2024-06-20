#pragma once

#include <memory>
#include <mutex>
#include <vector>

namespace ignosi::memory::detail {

class DllObjectPool {
  mutable std::mutex m_PoolMutex;
  std::vector<void*> m_FreeObjects;
  std::vector<void*> m_AllocatedObjects;

  const size_t m_ObjectSize{0};
  const size_t m_PoolSize{0};

  std::vector<std::unique_ptr<std::uint8_t[]>> m_Buffers;
  size_t m_BuffersSize{0};

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
