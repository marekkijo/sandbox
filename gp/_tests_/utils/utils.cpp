#include "utils.hpp"

#include <gtest/gtest.h>

TimeoutThread::TimeoutThread(const std::chrono::milliseconds duration_ms)
    : thread_{[](const std::chrono::milliseconds duration_ms, const std::atomic_bool &finish_flag) {
                std::this_thread::sleep_for(duration_ms);
                if (!finish_flag.load()) {
                  FAIL() << "Timeout (" << duration_ms.count() << "ms)";
                }
              },
              std::chrono::milliseconds(duration_ms),
              std::ref(finish_flag_)} {}

TimeoutThread::~TimeoutThread() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

void TimeoutThread::finish() { finish_flag_.store(true); }
