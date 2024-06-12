#pragma once

#include <mutex>

#include "dll_object_pool.h"

namespace ignosi::memory::detail {

class DllObjectPool {
  struct Node {
    Node* Next;
    Node* Previous;
  };

  std::mutex m_PoolMutex;

  size_t m_ObjectSize;
  size_t m_PoolSize;
  size_t m_NodeSize;
  size_t m_AllocatedCount;

  std::uint8_t* m_pBuffer;
  Node* m_EndNode;

  Node* m_pFirstFull;
  Node* m_pFirstEmpty;

 public:
  DllObjectPool(size_t objectSize, size_t poolSize);
  ~DllObjectPool();

  void* Allocate();
  void Dealloate(void* pObj);

  size_t PoolSize() const { return m_PoolSize; }
  size_t AllocatedCount() const { return m_AllocatedCount; }

 private:
  Node* findPrevious(Node* pFirst, Node* pCurrent);
};

}  // namespace ignosi::memory::detail
