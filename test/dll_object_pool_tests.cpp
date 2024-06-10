#include <fmt/format.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <dll_object_pool.hpp>
#include <queue>

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

class DllObjectPoolFixture : public testing::Test {
 public:
  DllObjectPoolFixture() {}

 protected:
  static constexpr std::uint32_t kBucketSize = 64;
  DllObjectPool<Data> m_Pool;
};

TEST_F(DllObjectPoolFixture, ValidateConstruction) {}

TEST_F(DllObjectPoolFixture, CreateDestroySingle) {
  auto obj = m_Pool.Create(1, 1.25);
  m_Pool.Destroy(obj);
}

TEST_F(DllObjectPoolFixture, CreateDestroyMany) {
  std::queue<Data*> objs;

  size_t val1 = 0;
  double val2 = 1;

  size_t val1_2 = 0;
  double val2_2 = 1;

  for (size_t i = 0; i < 5; ++i) {
    for (std::uint32_t i = 0; i < kBucketSize; ++i)
      objs.push(m_Pool.Create(val1++, val2++));
  }

  while (!objs.empty()) {
    auto obj = objs.front();
    objs.pop();
    Data& data = *obj;

    ASSERT_EQ(val1_2, obj->Value1);
    ASSERT_EQ(val2_2, obj->Value2);

    ASSERT_EQ(val1_2, (*obj).Value1);
    ASSERT_EQ(val2_2, (*obj).Value2);

    val1_2++;
    val2_2++;

    m_Pool.Destroy(obj);
  }
}
/*
TEST_F(DllObjectPoolFixture, ValidateIteratorLinear) {
  std::vector<Data> objects;
  for (int i = 0; i < 10; ++i) {
    objects.push_back(m_Pool.Create(i, i));
  }

  int i = 0;

  for (const auto& it : m_Pool) {
    ++i;
    auto is_equal = [&](const PoolPointer<Data>& pObj) {
      const Data& data = *pObj;
      return data == it;
    };

    ASSERT_NE(objects.end(),
              std::find_if(objects.begin(), objects.end(), is_equal));
  }

  ASSERT_EQ(i, 10);
}

TEST_F(DllObjectPoolFixture, ValidateIteratorNonLinear) {
  std::vector<PoolPointer<Data>> objects;
  for (int i = 0; i < 10; ++i) {
    auto pObj = m_Pool.Create(i, i);
    if (i != 2 && i != 6) objects.push_back(std::move(pObj));
  }

  int i = 0;

  for (const auto& it : m_Pool) {
    ++i;
    auto is_equal = [&](const PoolPointer<Data>& pObj) {
      const Data& data = *pObj;
      return data == it;
    };

    ASSERT_NE(objects.end(),
              std::find_if(objects.begin(), objects.end(), is_equal));
  }

  ASSERT_EQ(i, 8);
}
*/
}  // namespace ignosi::memory::test
