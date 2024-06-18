#include "dll_object_pool_data.h"

#include <dll_allocate.h>

#include <cassert>

namespace ignosi::memory::detail {

DllObjectPool::DllObjectPool(size_t objectSize, size_t poolSize)
    : m_ObjectSize(objectSize),
      m_PoolSize(poolSize),
      m_NodeSize(objectSize + sizeof(Node)) {
  initializeNewBufferBlock();
  m_pFirstFull = m_EndNode;
}

DllObjectPool::~DllObjectPool() {
  std::unique_lock<std::mutex> lock(m_PoolMutex);
  for (size_t i = 0; i < m_BuffersSize; ++i) {
    if (m_pBuffers[i]) {
      IgnosiMemoryDeallocate(m_pBuffers[i]);
    }
  }
  IgnosiMemoryDeallocate(m_pBuffers);
}

void* DllObjectPool::Allocate() {
  std::unique_lock<std::mutex> lock(m_PoolMutex);

  if (m_pFirstEmpty == m_EndNode) {
    initializeNewBufferBlock();
  }

  void* pNew = reinterpret_cast<std::uint8_t*>(m_pFirstEmpty) + sizeof(Node);
  Node* pNewNode = m_pFirstEmpty;
  m_pFirstEmpty = m_pFirstEmpty->Next();

  if (m_pFirstFull == m_EndNode) {
    pNewNode->Next(m_EndNode);
    m_pFirstFull = pNewNode;
  } else if (isBefore(pNewNode, m_pFirstFull)) {
    pNewNode->Next(m_pFirstFull);
    m_pFirstFull = pNewNode;
  } else {
    Node* pPrevFull = findPrevious(m_pFirstFull, m_pFirstEmpty);
    pNewNode->Next(pPrevFull->Next());
    pPrevFull->Next(pNewNode);
  }
  m_AllocatedCount++;

  return pNew;
}

void DllObjectPool::Dealloate(void* pObj) {
  std::unique_lock<std::mutex> lock(m_PoolMutex);

  Node* pToDestroy = reinterpret_cast<Node*>(
      reinterpret_cast<std::uint8_t*>(pObj) - sizeof(Node));
  if (pToDestroy == m_pFirstFull) {
    m_pFirstFull = pToDestroy->Next();
  } else {
    Node* pPrevFull = findPrevious(m_pFirstFull, pToDestroy);
    pPrevFull->Next(pToDestroy->Next());
  }

  if (m_pFirstEmpty == m_EndNode) {
    pToDestroy->Next(m_EndNode);
    m_pFirstEmpty = pToDestroy;
  } else if (isBefore(pToDestroy, m_pFirstEmpty)) {
    pToDestroy->Next(m_pFirstEmpty);
    m_pFirstEmpty = pToDestroy;
  } else {
    Node* pPrevEmpty = findPrevious(m_pFirstEmpty, pToDestroy);
    pToDestroy->Next(pPrevEmpty->Next());
    pPrevEmpty->Next(pToDestroy);
  }
  m_AllocatedCount--;
}

DllObjectPool::Node* DllObjectPool::findPrevious(Node* pFirst, Node* pCurrent) {
  Node* pPrevious = pFirst;
  while (isBefore(pPrevious->Next(), pCurrent)) {
    pPrevious = pPrevious->Next();
  }

  return pPrevious;
}

void DllObjectPool::initializeNewBufferBlock() {
  if (m_NextBlockLocation >= m_BuffersSize) {
    increaseBufferBlocksSize();
  }

  assert(m_pBuffers[m_NextBlockLocation] == nullptr);

  m_pBuffers[m_NextBlockLocation] =
      static_cast<std::uint8_t*>(IgnosiMemoryAllocate(m_NodeSize * m_PoolSize));

  m_EndNode = reinterpret_cast<Node*>(m_pBuffers[m_NextBlockLocation] +
                                      (m_PoolSize * m_NodeSize));

  if (m_NextBlockLocation > 0) {
    Node* pPreviousBack = reinterpret_cast<Node*>(
        m_pBuffers[m_NextBlockLocation - 1] + ((m_PoolSize - 1) * m_NodeSize));

    pPreviousBack->Next(m_EndNode);
  }

  m_pFirstEmpty = reinterpret_cast<Node*>(m_pBuffers[m_NextBlockLocation]);
  m_pFirstEmpty->Next(
      reinterpret_cast<Node*>(m_pBuffers[m_NextBlockLocation] + m_NodeSize));

  Node* pNext = m_pFirstEmpty->Next();
  for (size_t i = 1; i < m_PoolSize; ++i) {
    pNext->Next(reinterpret_cast<Node*>(m_pBuffers[m_NextBlockLocation] +
                                        m_NodeSize * (i + 1)));
    pNext = pNext->Next();
  }
  assert(pNext == m_EndNode);

  m_NextBlockLocation++;
}

void DllObjectPool::increaseBufferBlocksSize() {
  size_t newBuffersSize = m_BuffersSize + 8;
  std::uint8_t** pBuffers = static_cast<std::uint8_t**>(
      IgnosiMemoryAllocate(sizeof(std::uint8_t*) * newBuffersSize));

  std::fill(pBuffers, pBuffers + newBuffersSize, nullptr);
  if (m_pBuffers) {
    std::copy(m_pBuffers, m_pBuffers + m_BuffersSize, pBuffers);
  }

  std::uint8_t** oldBuffer = m_pBuffers;
  m_pBuffers = pBuffers;
  m_BuffersSize = newBuffersSize;

  if (oldBuffer) {
    IgnosiMemoryDeallocate(oldBuffer);
  }
}

bool DllObjectPool::isBefore(Node* lhs, Node* rhs) {
  if (lhs == m_EndNode) {
    return false;
  }
  if (rhs == m_EndNode) {
    return true;
  }
  std::uint8_t* pBufferlhs = findBuffer(lhs);
  std::uint8_t* pBufferrhs = findBuffer(rhs);
  if (pBufferlhs == pBufferrhs) {
    return lhs < rhs;
  }
  return pBufferlhs < pBufferrhs;
}

std::uint8_t* DllObjectPool::findBuffer(Node* pNode) {
  std::uint8_t* pNodeLoc = reinterpret_cast<std::uint8_t*>(pNode);
  for (size_t i = 0; i < m_BuffersSize; ++i) {
    std::uint8_t* pBuffer = m_pBuffers[i];
    assert(pBuffer);
    std::uint8_t* pBufferEnd = m_pBuffers[i] + (m_NodeSize * m_PoolSize);
    if (pNodeLoc >= pBuffer && pNodeLoc < pBufferEnd) {
      return pBuffer;
    }
  }
  return nullptr;
}

void DllObjectPool::Node::Next(Node* pNode) {
  assert(pNode != this);
  m_Next = pNode;
}

}  // namespace ignosi::memory::detail
