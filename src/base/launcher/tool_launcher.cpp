#include "tool_launcher.h"

namespace idk::base {

std::atomic_flag ToolLauncherContext::stopped = ATOMIC_FLAG_INIT;
int ToolLauncherContext::signal_pipe_fd[2];

}
