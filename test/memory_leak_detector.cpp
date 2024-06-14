#include "memory_leak_detector.h"

#ifdef _WIN32

MemoryLeakDetectorFixture::MemoryLeakDetectorFixture() {
  _CrtMemCheckpoint(&memState_);
}

MemoryLeakDetectorFixture::~MemoryLeakDetectorFixture() {
  _CrtMemState stateNow, stateDiff;
  _CrtMemCheckpoint(&stateNow);
  int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
  if (diffResult) report_failure(stateDiff.lSizes[1]);
}

void MemoryLeakDetectorFixture::report_failure(unsigned int unfreedBytes) {
  FAIL() << "Memory leak of " << unfreedBytes << " byte(s) detected.";
}
#else

#endif
