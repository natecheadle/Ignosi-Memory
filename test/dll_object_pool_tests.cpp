#include <fmt/format.h>
#include <gtest/gtest.h>

#include <dll_object_pool.hpp>
#include <future>
#include <iostream>
#include <vector>

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

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFull) {
  std::vector<Data*> objs;

  for (size_t i = 0; i < kPoolSize; ++i) {
    objs.push_back(m_Pool.Create(Data(i, (double)i)));
  }
  for (auto obj : objs) {
    ASSERT_NE(obj, nullptr);
  }
  ASSERT_EQ(m_Pool.Create(Data(11, 11.0)), nullptr);
  for (auto obj : objs) {
    m_Pool.Destroy(obj);
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFullMultiple) {
  for (int i = 0; i < 5; ++i) {
    std::vector<Data*> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto obj : objs) {
      ASSERT_NE(obj, nullptr);
    }
    ASSERT_EQ(m_Pool.Create(Data(11, 11.0)), nullptr);
    for (auto obj : objs) {
      m_Pool.Destroy(obj);
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyReverseOrder) {
  for (int i = 0; i < 5; ++i) {
    std::vector<Data*> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto obj : objs) {
      ASSERT_NE(obj, nullptr);
    }
    ASSERT_EQ(m_Pool.Create(Data(11, 11.0)), nullptr);
    for (auto it = objs.rbegin(); it != objs.rend(); ++it) {
      m_Pool.Destroy(*it);
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyOutOfOrder) {
  for (int i = 0; i < 5; ++i) {
    std::vector<Data*> objs;

    for (size_t i = 0; i < kPoolSize; ++i) {
      objs.push_back(m_Pool.Create(Data(i, (double)i)));
    }
    for (auto obj : objs) {
      ASSERT_NE(obj, nullptr);
    }
    ASSERT_EQ(m_Pool.Create(Data(11, 11.0)), nullptr);
    for (size_t i = 0; i < kPoolSize; ++i) {
      if (i % 2 == 0) {
        m_Pool.Destroy(objs[i]);
        objs[i] = nullptr;
      }
    }

    for (size_t i = 0; i < kPoolSize; ++i) {
      if (objs[i]) m_Pool.Destroy(objs[i]);
      objs[i] = nullptr;
    }
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFullMultipleThreads) {
  auto createDestroy = [&]() -> bool {
    try {
      std::vector<Data*> objs;

      for (size_t i = 0; i < kPoolSize / 2; ++i) {
        objs.push_back(m_Pool.Create(Data(i, (double)i)));
      }
      for (auto obj : objs) {
        if (obj == nullptr) {
          return false;
        }
      }
      for (auto obj : objs) {
        m_Pool.Destroy(obj);
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
}  // namespace ignosi::memory::test
