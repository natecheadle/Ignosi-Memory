#include <dll_allocate.h>
#include <fmt/format.h>
#include <gtest/gtest.h>

#include <dll_allocator.hpp>
#include <dll_pointer.hpp>
#include <stdexcept>

#include "memory_leak_detector.h"

namespace Ignosi::test {
namespace {

struct TestObject;

struct TestObject {
  TestObject() : Value1(0.0), Value2(0.0) {}
  TestObject(double value1, double value2) : Value1(value1), Value2(value2) {}

  TestObject(const TestObject& other) = default;
  TestObject(TestObject&& other) = default;

  TestObject& operator=(const TestObject& other) = default;
  TestObject& operator=(TestObject&& other) = default;

  double Value1;
  double Value2;

  friend bool operator==(const TestObject& lhs,
                         const TestObject& rhs) = default;
  friend bool operator!=(const TestObject& lhs,
                         const TestObject& rhs) = default;

  friend std::ostream& operator<<(std::ostream& os, const TestObject& value) {
    os << fmt::format("Value1 = {}, Value2 = {}", value.Value1, value.Value2);
    return os;
  }
};

struct ThrowingTestObject : public TestObject {
  ThrowingTestObject() { throw std::runtime_error("ERROR"); }
};

struct DerivedTestObject : public TestObject {
  DerivedTestObject() = default;

  DerivedTestObject(double value1, double value2, double value3)
      : TestObject(value1, value2) {
    Value3 = value3;
  }

  DerivedTestObject(const DerivedTestObject& other) = default;
  DerivedTestObject(DerivedTestObject&& other) = default;

  DerivedTestObject& operator=(const DerivedTestObject& other) = default;
  DerivedTestObject& operator=(DerivedTestObject&& other) = default;

  double Value3;

  friend bool operator==(const DerivedTestObject& lhs,
                         const DerivedTestObject& rhs) = default;
  friend bool operator!=(const DerivedTestObject& lhs,
                         const DerivedTestObject& rhs) = default;

  friend std::ostream& operator<<(std::ostream& os,
                                  const DerivedTestObject& value) {
    os << static_cast<const TestObject&>(value)
       << fmt::format(", Value3 = {}", value.Value3);
    return os;
  }
};

};  // namespace

class dll_allocate_fixture : public memory_leak_detector_fixture {};

TEST_F(dll_allocate_fixture, validate_create_destroy) {
  ASSERT_NO_THROW(dll::make_unique_dll_obj<TestObject>());
}

TEST_F(dll_allocate_fixture, validate_create_destroy_args) {
  dll::dll_pointer<TestObject> ptr =
      dll::make_unique_dll_obj<TestObject>(1.0, 2.0);

  ASSERT_EQ(*ptr, TestObject(1.0, 2.0));
}

TEST_F(dll_allocate_fixture, validate_create_throwing_object) {
  ASSERT_THROW(dll::make_unique_dll_obj<ThrowingTestObject>(),
               std::runtime_error);
}

TEST_F(dll_allocate_fixture, validate_convert_unique_ptr) {
  dll::dll_pointer<TestObject> pObj = dll::make_unique_dll_obj<TestObject>();
  dll::dll_unique_ptr<TestObject> pObjUnique(
      pObj.release(), &(dll::dll_pointer<TestObject>::destroy));

  ASSERT_EQ(pObj.get(), nullptr);
  ASSERT_NE(pObjUnique.get(), nullptr);
}

TEST_F(dll_allocate_fixture, validate_create_derived) {
  dll::dll_pointer<TestObject> pObj =
      dll::make_unique_dll_obj<DerivedTestObject>();
}

TEST_F(dll_allocate_fixture, validate_create_shared_derived) {
  dll::dll_pointer<DerivedTestObject> pObj =
      dll::make_unique_dll_obj<DerivedTestObject>(1.0, 2.0, 3.0);
  std::shared_ptr<DerivedTestObject> pObjShared1(
      pObj.release(), &(dll::dll_pointer<TestObject>::destroy));
  std::shared_ptr<TestObject> pObjShared2(pObjShared1);

  pObjShared1->Value1 = 10.0;
  pObjShared1->Value2 = 20.0;
  pObjShared1->Value3 = 30.0;

  ASSERT_EQ(pObjShared2->Value1, 10.0);
  ASSERT_EQ(pObjShared2->Value2, 20.0);
}

TEST_F(dll_allocate_fixture, validate_create_dll_allocator) {
  std::vector<TestObject, dll::dll_allocator<TestObject>> testVector;
  for (double i = 0.0; i < 10.0; ++i) {
    ASSERT_NO_THROW(testVector.push_back(TestObject(i, i * 2.0)));
  }
}
}  // namespace Ignosi::test