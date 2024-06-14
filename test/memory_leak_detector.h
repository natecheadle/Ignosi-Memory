#include <gtest/gtest.h>

#ifdef _WIN32

#include <crtdbg.h>

class MemoryLeakDetectorFixture : public testing::Test {
  _CrtMemState memState_;

 public:
  MemoryLeakDetectorFixture();
  ~MemoryLeakDetectorFixture();

 private:
  void report_failure(unsigned int unfreedBytes);
};
#else

class MemoryLeakDetectorFixture : public testing::Test {};
#endif
