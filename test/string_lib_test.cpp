#include <gtest/gtest.h>
#include <string_lib.h>

namespace my::test::app {
TEST(string_lib_fixture, validate_to_upper) {
  ASSERT_EQ(lib::to_upper("hello"), "HELLO");
}
}  // namespace my::test::app
