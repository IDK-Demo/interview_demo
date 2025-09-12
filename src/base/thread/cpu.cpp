#include "cpu.h"

#include <iostream>
#include <pthread.h>

#include "base/logger/logger.h"

namespace idk::base {

void Cpu::bind_this_thread_to_cpu(int cpu_id) {
#ifdef __unix__
  if (cpu_id < 0 || cpu_id >= sysconf(_SC_NPROCESSORS_ONLN)) {
    ERROR("cpu_id {} is not in valid range [0, {}]. Ignoring", cpu_id,
            sysconf(_SC_NPROCESSORS_ONLN));
    return;
  }
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_id, &cpuset);
  int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
  if (rc != 0) {
    std::cout << "Error calling pthread_setaffinity_np: " << rc << std::endl;;
  }
#else
  ERROR("Failed to bind this thread to cpu {}, not supported", cpu_id);
#endif
}

}
