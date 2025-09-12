#include "gateway/ws_demo/service.h"

using namespace idk;

int
main(int argc, const char** argv) {
  base::Logger logger(base::Logger::Params{});
  return WsDemoService(argc, argv).run();
}


