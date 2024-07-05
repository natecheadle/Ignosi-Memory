#include "dll_object_pool_data.h"

#include <fmt/format.h>

#include <cassert>

namespace ignosi::memory::detail {

DllObjectPool::DllObjectPool(size_t objectSize, size_t poolSize)
    : m_FreeObjects(poolSize), m_ObjectSize(objectSize), m_PoolSize(poolSize) {
  initializeNewBufferBlock();
}

DllObjectPool::~DllObjectPool() {
  m_BuffersSize = m_Buffers.size();
  for (size_t i = 0; i < m_Buffers.size(); ++i) {
    m_Buffers[i].reset();
  }
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
size_t DllObjectPool::MaxAllocatedCount() const {
  return m_PoolSize * m_Buffers.size();
}

void DllObjectPool::initializeNewBufferBlock() {
  size_t newLoc = m_BuffersSize++;
  if (newLoc >= m_Buffers.size()) {
    fmt::print("Max capacity of {} objects reached",
               m_PoolSize * m_Buffers.size());
    return;
  }

  m_Buffers[newLoc] = std::unique_ptr<std::uint8_t[]>(
      new std::uint8_t[m_PoolSize * m_ObjectSize]);
  m_FreeObjects.reserve(m_Buffers.size() * m_PoolSize);

  std::uint8_t* pNewBuffer = m_Buffers[newLoc].get();
  assert(pNewBuffer);
  for (size_t i = 0; i < m_PoolSize; ++i) {
    m_FreeObjects.push(pNewBuffer + (i * m_ObjectSize));
  }
}

}  // namespace ignosi::memory::detail
