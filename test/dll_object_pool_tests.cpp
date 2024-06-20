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
  static constexpr size_t kPoolSize = 4;
  DllObjectPool<Data> m_Pool{kPoolSize};

  std::vector<Data> m_ExpectedData;
  std::vector<DllUniquePtr<Data>> m_ActualData;

  void fillData(size_t fillSize) {
    for (size_t i = 0; i < fillSize; ++i) {
      m_ExpectedData.push_back(Data(i, (double)i * (double)i));
      m_ActualData.push_back(m_Pool.Create(m_ExpectedData.back()));
      ASSERT_EQ(m_Pool.AllocatedCount(), i + 1);
    }
  }

  void validateData() {
    ASSERT_EQ(m_ExpectedData.size(), m_ActualData.size());
    for (size_t i = 0; i < m_ExpectedData.size(); ++i) {
      ASSERT_NE(m_ActualData[i].get(), nullptr);
      ASSERT_EQ(*m_ActualData[i], m_ExpectedData[i]);
    }
  }

  void clearData() {
    m_ExpectedData.clear();
    m_ActualData.clear();
  }
};

TEST_F(DllObjectPoolFixture, ValidateConstruction) {}

TEST_F(DllObjectPoolFixture, ValidatePoolSize) {
  ASSERT_EQ(m_Pool.PoolSize(), kPoolSize);
}

TEST_F(DllObjectPoolFixture, ValidateAllocatedCount) { fillData(kPoolSize); }

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFull) {
  fillData(kPoolSize);
  validateData();
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyTillFullMultiple) {
  for (int j = 0; j < 2; ++j) {
    fillData(kPoolSize);
    validateData();
    clearData();
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyReverseOrder) {
  for (int j = 0; j < 2; ++j) {
    fillData(kPoolSize);
    for (auto it = m_ActualData.rbegin(); it != m_ActualData.rend(); ++it) {
      it->reset();
    }
    clearData();
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyOutOfOrder) {
  for (int j = 0; j < 2; ++j) {
    fillData(kPoolSize);
    validateData();
    for (size_t i = 0; i < m_ActualData.size(); ++i) {
      if (i % 2 == 0) {
        m_ActualData[i].reset();
      }
    }

    for (size_t i = 0; i < m_ActualData.size(); ++i) {
      m_ActualData[i] = m_Pool.Create(Data{i, (double)i * (double)i});
    }
    validateData();

    clearData();
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
  fillData(kPoolSize * 4);
  validateData();
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyOutOfOrderPastFull) {
  for (size_t i = 0; i < 2; ++i) {
    fillData(kPoolSize * 4);
    validateData();
    for (size_t j = 0; j < m_ActualData.size(); ++j) {
      if (j % 2 == 0) {
        m_ActualData[j].reset();
      }
    }

    clearData();
  }
}

TEST_F(DllObjectPoolFixture, ValidateCreateDestroyPastFullMultipleThreads) {
  auto createDestroy = [&]() -> bool {
    try {
      std::vector<DllUniquePtr<Data>> objs;

      for (size_t i = 0; i < kPoolSize * 2; ++i) {
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

}  // namespace ignosi::memory::test
