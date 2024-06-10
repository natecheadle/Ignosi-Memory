#pragma once

#include <array>
#include <cstdint>
#include <dll_allocator.hpp>
#include <limits>
#include <mutex>
#include <optional>
#include <vector>

namespace ignosi::memory {

template <typename T>
class DllObjectPool {
 public:
  struct Address {
    std::uint32_t bucket;
    std::uint32_t index;

    Address& decrement(std::uint32_t bucket_size) {
      if (index == 0 && bucket == 0) {
        *this = kNullAddress;
      } else if (index == 0) {
        --bucket;
        index = bucket_size;
      } else {
        --index;
      }
      return *this;
    }

    Address& increment(std::uint32_t bucket_size) {
      if (index >= bucket_size - 1) {
        index = 0;
        bucket++;
      } else {
        ++index;
      }
      return *this;
    }

    friend bool operator==(const Address& lhs, const Address& rhs) = default;
    friend bool operator!=(const Address& lhs, const Address& rhs) = default;
    friend bool operator<(const Address& lhs, const Address& rhs) {
      return lhs.bucket < rhs.bucket && lhs.index < rhs.index;
    }
    friend bool operator>(const Address& lhs, const Address& rhs) {
      return lhs.bucket > rhs.bucket && lhs.index > rhs.index;
    }
    friend bool operator<=(const Address& lhs, const Address& rhs) {
      return lhs == rhs || lhs < rhs;
    }
    friend bool operator>=(const Address& lhs, const Address& rhs) {
      return lhs == rhs || lhs > rhs;
    }
  };

 private:
  union obj {
    obj() {}
    T object;
    std::array<std::uint8_t, sizeof(T)> bytes;
  };

  struct Data {
    Data() : is_alive(false), previous(kNullAddress), next(kNullAddress) {}
    obj obj_data;
    bool is_alive;
    Address previous;
    Address next;
  };

  static constexpr std::uint16_t kDefaultBucketSize = 256;
  static constexpr Address kNullAddress =
      Address{.bucket = std::numeric_limits<std::uint32_t>::max(),
              .index = std::numeric_limits<std::uint32_t>::max()};

  std::mutex m_pool_mutex;
  std::uint32_t m_bucket_size;
  std::vector<std::vector<Data, DllAllocator<Data>>,
              DllAllocator<std::vector<Data, DllAllocator<Data>>>>
      m_objects;

  Address m_first_empty;
  Address m_first_occupied;

 public:
  DllObjectPool(std::uint32_t bucket_size)
      : m_bucket_size(bucket_size),
        m_first_empty(0, 0),
        m_first_occupied(m_objects.size(), bucket_size) {
    m_objects.resize(1);
    m_objects[0].resize(bucket_size);
  }

  DllObjectPool() : DllObjectPool(kDefaultBucketSize) {}

  std::optional<T>& At(const Address& address) {
    return m_objects.at(address.bucket).at(address.index);
  }

  const std::optional<T>& At(const Address& address) const {
    return m_objects.at(address.bucket).at(address.index);
  }

  template <typename... Args>
  T* Create(Args&&... args) {
    return Create(T(std::forward<Args>(args)...));
  }

  T* Create(const T& obj) { return Create(T(obj)); }

  T* Create(T&& obj) {
    std::unique_lock<std::mutex> lock(m_pool_mutex);
    if (m_first_empty == kNullAddress) {
      expand();
    }

    const Address& newObjLocation = m_first_empty;
    Data& newObjData = m_objects[newObjLocation.bucket][newObjLocation.index];

    new (static_cast<void*>(newObjData.obj_data.bytes.data()))
        T(std::forward<T>(obj));

    Address previousFull{0, 0};
    Address nextFull{0, 0};

    if (m_first_occupied == kNullAddress) {
      previousFull = kNullAddress;
      nextFull = kNullAddress;
    } else {
      previousFull = findPrevious(m_first_occupied, newObjLocation);
      nextFull = newObjData.next;
    }

    Address new_first_empty =
        m_objects[newObjLocation.bucket][newObjLocation.index].next;

    newObjData.next = nextFull;
    newObjData.is_alive = true;

    if (previousFull != kNullAddress) {
      m_objects[previousFull.bucket][previousFull.index].next = newObjLocation;
    }
    m_first_empty = new_first_empty;
    if (newObjLocation < m_first_occupied) {
      m_first_occupied = newObjLocation;
    }

    return &newObjData.obj_data.object;
  }

  void Destroy(T* pObject) {
    std::unique_lock<std::mutex> lock(m_pool_mutex);

    Data* pDataObject = reinterpret_cast<Data*>(pObject);
    pDataObject->obj_data.object.~T();
    pDataObject->is_alive = false;

    Address address_to_destroy = getAddress(pDataObject);

    if (address_to_destroy == m_first_occupied) {
      m_first_occupied = pDataObject->next;
      m_objects[m_first_occupied.bucket][m_first_occupied.index].previous =
          kNullAddress;
    } else {
      const Address& previousFull = pDataObject->previous;
      m_objects[previousFull.bucket][previousFull.index].next =
          pDataObject->next;
    }

    if (address_to_destroy < m_first_empty) {
      pDataObject->next = m_first_empty;
      m_first_empty = address_to_destroy;
    } else {
      Address previous_empty = findPrevious(m_first_empty, address_to_destroy);

      pDataObject->next =
          m_objects[previous_empty.bucket][previous_empty.index].next;
      m_objects[previous_empty.bucket][previous_empty.index].next =
          address_to_destroy;
    }
  }

 private:
  void expand() {
    std::uint32_t newBucket = m_objects.size();
    m_objects.resize(m_objects.size() + 1);

    m_first_empty = {newBucket, 0};
    std::vector<Data, DllAllocator<Data>>& bucket = m_objects.at(newBucket);
    bucket.resize(m_bucket_size);

    bucket.front().next = {newBucket, 1};
    bucket.front().previous = kNullAddress;

    bucket.back().next = kNullAddress;
    bucket.back().previous = {newBucket, m_bucket_size - 2};

    for (std::uint32_t i = 1; i < m_bucket_size - 1; ++i) {
      bucket[i].next = {newBucket, i + 1};
      bucket[i].previous = {newBucket, i - 1};
    }
  }

  Address getAddress(Data* data) {
    return m_objects[data->previous.bucket][data->previous.index].next;
  }

  Address findPrevious(const Address& first, const Address& current) {
    Address previous = first;
    while (previous != kNullAddress) {
      Address next = m_objects[previous.bucket][previous.index].next;
      if (next < current) {
        previous = next;
      } else {
        break;
      }
    }
    return previous;
  }
};

}  // namespace ignosi::memory
