#pragma once

namespace idk::base {

class Cpu {
public:
  static constexpr int kLowPriorityCpuId = 0;
  static void bind_this_thread_to_cpu(int core_id);
};

}
