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
}  // namespace ignosi::memory::test
