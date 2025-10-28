#pragma once

#include <atomic>
#include <chrono>
#include <thread>

class TimeoutThread {
public:
  explicit TimeoutThread(const std::chrono::milliseconds duration_ms = std::chrono::seconds{1});
  ~TimeoutThread();

  void finish();

private:
  std::atomic_bool finish_flag_{false};
  std::thread thread_;
};
