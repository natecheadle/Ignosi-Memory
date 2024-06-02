#include <gtest/gtest.h>

#ifdef _WIN32

#include <crtdbg.h>

class memory_leak_detector_fixture : public testing::Test {
  _CrtMemState memState_;

 public:
  memory_leak_detector_fixture() { _CrtMemCheckpoint(&memState_); }

  ~memory_leak_detector_fixture() {
    _CrtMemState stateNow, stateDiff;
    _CrtMemCheckpoint(&stateNow);
    int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
    if (diffResult) report_failure(stateDiff.lSizes[1]);
  }

 private:
  void report_failure(unsigned int unfreedBytes) {
    FAIL() << "Memory leak of " << unfreedBytes << " byte(s) detected.";
  }
};
#else

class memory_leak_detector_fixture : public testing::Test {};
#endif
