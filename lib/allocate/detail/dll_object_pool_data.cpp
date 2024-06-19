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
    void* pNew = m_FreeObjects.front();
    m_FreeObjects.pop();

    m_AllocatedObjects.push_back(pNew);
    m_AllocatedObjects.sort();

    return pNew;
  } catch (const std::exception& ex) {
    fmt::print("{}", ex.what());
  }
  return nullptr;
}

void DllObjectPool::Dealloate(void* pObj) {
  if (pObj == nullptr) {
    return;
  }
  try {
    std::unique_lock<std::mutex> lock(m_PoolMutex);
    auto objToRemove = std::lower_bound(m_AllocatedObjects.begin(),
                                        m_AllocatedObjects.end(), pObj);
    assert(objToRemove != m_AllocatedObjects.end());

    m_AllocatedObjects.erase(objToRemove);
    m_FreeObjects.push(pObj);
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
  std::uint8_t* pNewBuffer = m_Buffers.back().get();
  for (size_t i = 0; i < m_PoolSize; ++i) {
    m_FreeObjects.push(pNewBuffer + (i * m_ObjectSize));
  }
}

}  // namespace ignosi::memory::detail
