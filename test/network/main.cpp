#include <gtest/gtest.h>

#include "base/logger/logger.h"

using namespace idk::base;

class MyEnvironment : public ::testing::Environment {
public:
  void SetUp() override {
    logger.emplace();
  }

  void TearDown() override {
    logger.reset();
  }
private:
  std::optional<Logger> logger;
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Register the environment
  ::testing::AddGlobalTestEnvironment(new MyEnvironment);

  return RUN_ALL_TESTS();
}