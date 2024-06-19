#include "dll_object_pool_data.h"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <iostream>

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

    m_AllocatedObjects.push_back(pNew);
    std::sort(m_AllocatedObjects.begin(), m_AllocatedObjects.end());

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
    auto objToRemove = std::lower_bound(m_AllocatedObjects.begin(),
                                        m_AllocatedObjects.end(), pObj);
    if (objToRemove == m_AllocatedObjects.end()) {
      throw std::runtime_error("Object is not in allocated objects list");
    }

    m_AllocatedObjects.erase(objToRemove);
    m_FreeObjects.push_back(pObj);
  } catch (std::exception& ex) {
    fmt::print("{}", ex.what());
  }
}

size_t DllObjectPool::PoolSize() const { return m_PoolSize; }
size_t DllObjectPool::AllocatedCount() const {
  std::unique_lock<std::mutex> lock(m_PoolMutex);
  return m_AllocatedObjects.size();
}

void DllObjectPool::initializeNewBufferBlock() {
  m_Buffers.push_back(std::unique_ptr<std::uint8_t[]>(
      new std::uint8_t[m_PoolSize * m_ObjectSize]));
  m_AllocatedObjects.reserve(m_Buffers.size() * m_PoolSize);
  m_FreeObjects.reserve(m_Buffers.size() * m_PoolSize);

  std::uint8_t* pNewBuffer = m_Buffers.back().get();
  for (size_t i = 0; i < m_PoolSize; ++i) {
    m_FreeObjects.push_back(pNewBuffer + (i * m_ObjectSize));
  }
}

}  // namespace ignosi::memory::detail
