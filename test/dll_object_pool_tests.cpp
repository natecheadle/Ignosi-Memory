#include <fmt/format.h>
#include <gtest/gtest.h>

#include <dll_object_pool.hpp>
#include <future>
#include <iostream>
#include <vector>

#include "dll_unique_ptr.hpp"
#include "memory_leak_detector.h"

namespace ignosi::memory::test {
namespace {
struct Data {
  Data() {}

  Data(size_t value1, double value2) : Value1(value1), Value2(value2) {}

  size_t Value1{0};
  double Value2{0.0};

  friend bool operator==(const Data& lhs, const Data& rhs) = default;

  friend std::ostream& operator<<(std::ostream& os, const Data& value) {
    os << fmt::format("Value1: {} Value2: {}", value.Value1, value.Value2);
    return os;
  }
};

}  // namespace

class DllObjectPoolFixture : public MemoryLeakDetectorFixture {
 public:
  DllObjectPoolFixture() {}

 protected:
  static constexpr size_t kPoolSize = 8;
  DllObjectPool<Data> m_Pool{kPoolSize};
};

TEST_F(DllObjectPoolFixture, ValidateConstruction) {}

TEST_F(DllObjectPoolFixture, ValidatePoolSize) {
  ASSERT_EQ(m_Pool.PoolSize(), kPoolSize);
}

TEST_F(DllObjectPoolFixture, ValidateAllocatedCount) {
  std::vector<DllUniquePtr<Data>> objs;

  for (size_t i = 0; i < kPoolSize; ++i) {
    objs.push_back(m_Pool.Create(Data(i, (double)i)));
    ASSERT_EQ(m_Pool.AllocatedCount(), i + 1);
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFull) {
  std::vector<DllUniquePtr<Data>> objs;

  for (size_t i = 0; i < kPoolSize; ++i) {
    objs.push_back(m_Pool.Create(Data(i, (double)i)));
  }
  for (auto& obj : objs) {
    ASSERT_NE(obj.get(), nullptr);
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFullMultiple) {
  for (int j = 0; j < 5; ++j) {
    std::vector<DllUniquePtr<Data>> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto& obj : objs) {
      ASSERT_NE(obj.get(), nullptr);
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyReverseOrder) {
  for (int j = 0; j < 5; ++j) {
    std::vector<DllUniquePtr<Data>> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto& obj : objs) {
      ASSERT_NE(obj.get(), nullptr);
    }
    for (auto it = objs.rbegin(); it != objs.rend(); ++it) {
      it->reset();
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyOutOfOrder) {
  for (int j = 0; j < 5; ++j) {
    std::vector<DllUniquePtr<Data>> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto& obj : objs) {
      ASSERT_NE(obj, nullptr);
    }
    for (size_t i = 0; i < kPoolSize; ++i) {
      if (i % 2 == 0) {
        objs[i].reset();
      }
    }

    for (size_t i = 0; i < kPoolSize; ++i) {
      if (objs[i]) {
        objs[i].reset();
      }
      objs[i] = nullptr;
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFullMultipleThreads) {
  auto createDestroy = [&]() -> bool {
    try {
      std::vector<DllUniquePtr<Data>> objs;

      for (size_t i = 0; i < kPoolSize / 2; ++i) {
        objs.push_back(m_Pool.Create(Data(i, (double)i)));
      }
      for (auto& obj : objs) {
        if (obj == nullptr) {
          return false;
        }
      }
    } catch (...) {
      return false;
    }
    return true;
  };

  for (int i = 0; i < 5; ++i) {
    std::future thread1 = std::async(createDestroy);
    std::future thread2 = std::async(createDestroy);

    ASSERT_TRUE(thread1.get());
    ASSERT_TRUE(thread2.get());
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyPastFull) {
  std::vector<DllUniquePtr<Data>> objs;

  for (size_t i = 0; i < kPoolSize * 4; ++i) {
    objs.push_back(m_Pool.Create(Data(i, (double)i)));
  }
  for (auto& obj : objs) {
    ASSERT_NE(obj.get(), nullptr);
  }
}

}  // namespace ignosi::memory::test
