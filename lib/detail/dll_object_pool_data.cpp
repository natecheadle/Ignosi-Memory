#include "dll_object_pool_data.h"

#include <fmt/format.h>

#include <cassert>

namespace ignosi::memory::detail {

DllObjectPool::DllObjectPool(size_t objectSize, size_t poolSize)
    : m_ObjectSize(objectSize),
      m_PoolSize(poolSize),
      m_FreeObjects(poolSize),
      m_Buffers(256) {
  initializeNewBufferBlock();
}

void* DllObjectPool::Allocate() {
  try {
    void* pNew{nullptr};
    while (!m_FreeObjects.pop(pNew)) {
      initializeNewBufferBlock();
    }

    m_AllocatedCount++;
    return pNew;
  } catch (const std::exception& ex) {
    fmt::print("{}", ex.what());
  }
  return nullptr;
}

void DllObjectPool::Deallocate(void* pObj) {
  if (!pObj) {
    return;
  }
  try {
    m_FreeObjects.push(pObj);
    m_AllocatedCount--;
  } catch (std::exception& ex) {
    fmt::print("{}", ex.what());
  }
}

size_t DllObjectPool::PoolSize() const { return m_PoolSize; }
size_t DllObjectPool::AllocatedCount() const { return m_AllocatedCount; }

void DllObjectPool::initializeNewBufferBlock() {
  size_t newSize = ++m_BuffersSize;

  std::shared_ptr<std::uint8_t[]> pNewBufferUnique =
      std::shared_ptr<std::uint8_t[]>(
          new std::uint8_t[m_PoolSize * m_ObjectSize]);

  std::uint8_t* pNewBuffer = pNewBufferUnique.get();
  if (!pNewBuffer) {
    fmt::print("Failed to create new buffer");
    return;
  }

  m_FreeObjects.reserve(newSize * m_PoolSize);
  m_Buffers.push(std::move(pNewBufferUnique));
  for (size_t i = 0; i < m_PoolSize; ++i) {
    m_FreeObjects.push(pNewBuffer + (i * m_ObjectSize));
  }
}

}  // namespace ignosi::memory::detail
