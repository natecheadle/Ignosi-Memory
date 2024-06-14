#include "dll_object_pool_data.h"

#include <dll_allocate.h>

namespace ignosi::memory::detail {

DllObjectPool::DllObjectPool(size_t objectSize, size_t poolSize)
    : m_ObjectSize(objectSize),
      m_PoolSize(poolSize),
      m_NodeSize(objectSize + sizeof(Node)) {
  m_pBuffer =
      static_cast<std::uint8_t*>(IgnosiMemoryAllocate(m_NodeSize * m_PoolSize));

  m_EndNode = reinterpret_cast<Node*>(m_pBuffer + (m_PoolSize * m_NodeSize));
  m_pFirstFull = m_EndNode;

  m_pFirstEmpty = reinterpret_cast<Node*>(m_pBuffer);
  m_pFirstEmpty->Next = reinterpret_cast<Node*>(m_pBuffer + m_NodeSize);

  Node* pNext = m_pFirstEmpty->Next;
  for (size_t i = 1; i < m_PoolSize; ++i) {
    pNext->Next = reinterpret_cast<Node*>(m_pBuffer + m_NodeSize * (i + 1));
    pNext = pNext->Next;
  }
}

DllObjectPool::~DllObjectPool() {
  std::unique_lock<std::mutex> lock(m_PoolMutex);
  IgnosiMemoryDeallocate(m_pBuffer);
}

void* DllObjectPool::Allocate() {
  std::unique_lock<std::mutex> lock(m_PoolMutex);

  if (m_pFirstEmpty == m_EndNode) {
    return nullptr;
  }

  void* pNew = reinterpret_cast<std::uint8_t*>(m_pFirstEmpty) + sizeof(Node);
  Node* pNewNode = m_pFirstEmpty;
  m_pFirstEmpty = m_pFirstEmpty->Next;

  if (m_pFirstFull == m_EndNode) {
    pNewNode->Next = m_EndNode;
  } else if (pNewNode < m_pFirstFull) {
    pNewNode->Next = m_pFirstFull;
    m_pFirstFull = pNewNode;
  } else {
    Node* pPrevFull = findPrevious(m_pFirstFull, m_pFirstEmpty);
    pNewNode->Next = pPrevFull->Next;
    pPrevFull->Next = pNewNode;
  }
  m_AllocatedCount++;

  return pNew;
}

void DllObjectPool::Dealloate(void* pObj) {
  std::unique_lock<std::mutex> lock(m_PoolMutex);

  Node* pToDestroy = reinterpret_cast<Node*>(
      reinterpret_cast<std::uint8_t*>(pObj) - sizeof(Node));
  if (m_pFirstEmpty == m_EndNode) {
    pToDestroy->Next = m_EndNode;
    m_pFirstEmpty = pToDestroy;
  } else if (pToDestroy < m_pFirstEmpty) {
    pToDestroy->Next = m_pFirstEmpty;
    m_pFirstEmpty = pToDestroy;
  } else {
    Node* pPrevEmpty = findPrevious(m_pFirstEmpty, pToDestroy);
    pToDestroy->Next = pPrevEmpty->Next;
    pPrevEmpty->Next = pToDestroy;
  }
  m_AllocatedCount--;
}

DllObjectPool::Node* DllObjectPool::findPrevious(Node* pFirst, Node* pCurrent) {
  Node* pPrevious = pFirst;
  while (pPrevious->Next < pCurrent) {
    pPrevious = pPrevious->Next;
  }

  return pPrevious;
}

}  // namespace ignosi::memory::detail
