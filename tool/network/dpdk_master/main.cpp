#include "network/dpdk/master_service.h"

using namespace idk::base;

int
main(int argc, const char** argv) {
  Logger logger(Logger::Params{});
  return DpdkMasterService(argc, argv).run();
}
