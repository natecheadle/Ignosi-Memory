#include "dll_object_pool_data.h"

#include <fmt/format.h>

#include <cassert>

namespace ignosi::memory::detail {

DllObjectPool::DllObjectPool(size_t objectSize, size_t poolSize)
    : m_ObjectSize(objectSize), m_PoolSize(poolSize) {
  initializeNewBufferBlock();
}

DllObjectPool::~DllObjectPool() {
  std::unique_lock<std::mutex> lock(m_PoolMutex);
  m_Buffers.clear();
}

void* DllObjectPool::Allocate() {
  try {
    std::unique_lock<std::mutex> lock(m_PoolMutex);
    if (m_FreeObjects.empty()) {
      initializeNewBufferBlock();
    }
    void* pNew = m_FreeObjects.back();
    m_FreeObjects.pop_back();
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
    std::unique_lock<std::mutex> lock(m_PoolMutex);
    m_FreeObjects.push_back(pObj);
    m_AllocatedCount--;
  } catch (std::exception& ex) {
    fmt::print("{}", ex.what());
  }
}

size_t DllObjectPool::PoolSize() const { return m_PoolSize; }
size_t DllObjectPool::AllocatedCount() const {
  std::unique_lock<std::mutex> lock(m_PoolMutex);
  return m_AllocatedCount;
}

void DllObjectPool::initializeNewBufferBlock() {
  m_Buffers.push_back(std::unique_ptr<std::uint8_t[]>(
      new std::uint8_t[m_PoolSize * m_ObjectSize]));
  m_FreeObjects.reserve(m_Buffers.size() * m_PoolSize);

  std::uint8_t* pNewBuffer = m_Buffers.back().get();
  for (size_t i = 0; i < m_PoolSize; ++i) {
    m_FreeObjects.push_back(pNewBuffer + (i * m_ObjectSize));
  }
}

}  // namespace ignosi::memory::detail
