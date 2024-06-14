#pragma once

#include <mutex>

namespace ignosi::memory::detail {

class DllObjectPool {
  struct Node {
    Node* Next;
  };

  std::mutex m_PoolMutex;

  size_t m_ObjectSize{0};
  size_t m_PoolSize{0};
  size_t m_NodeSize{0};
  size_t m_AllocatedCount{0};

  std::uint8_t* m_pBuffer{nullptr};
  Node* m_EndNode{nullptr};

  Node* m_pFirstFull{nullptr};
  Node* m_pFirstEmpty{nullptr};

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
