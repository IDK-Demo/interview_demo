#pragma once
#include <thread>
#include <functional>

namespace idk::base {

class Thread {
public:
  Thread(std::function<void()> func) : thread_(std::move(func)) {}

  ~Thread() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }
private:
  std::thread thread_;
};

}