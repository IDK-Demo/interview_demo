#include <gtest/gtest.h>
#include "network/tcp/flags.h"

using namespace idk::net::tcp;

class TcpFlagsTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TcpFlagsTest, BasicFlagValues) {
  EXPECT_EQ(static_cast<uint8_t>(Flags::FIN), 0x01);
  EXPECT_EQ(static_cast<uint8_t>(Flags::SYN), 0x02);
  EXPECT_EQ(static_cast<uint8_t>(Flags::RST), 0x04);
  EXPECT_EQ(static_cast<uint8_t>(Flags::PSH), 0x08);
  EXPECT_EQ(static_cast<uint8_t>(Flags::ACK), 0x10);
  EXPECT_EQ(static_cast<uint8_t>(Flags::URG), 0x20);
  EXPECT_EQ(static_cast<uint8_t>(Flags::ECE), 0x40);
  EXPECT_EQ(static_cast<uint8_t>(Flags::CWR), 0x80);
}

TEST_F(TcpFlagsTest, CommonCombinations) {
  EXPECT_EQ(static_cast<uint8_t>(Flags::SYN_ACK), 0x12);
  EXPECT_EQ(static_cast<uint8_t>(Flags::FIN_ACK), 0x11);
  EXPECT_EQ(static_cast<uint8_t>(Flags::RST_ACK), 0x14);
  EXPECT_EQ(static_cast<uint8_t>(Flags::PSH_ACK), 0x18);
}

TEST_F(TcpFlagsTest, BitwiseOperations) {
  // Test OR operation
  Flags combined = Flags::SYN | Flags::ACK;
  EXPECT_EQ(static_cast<uint8_t>(combined), 0x12);
  EXPECT_EQ(combined, Flags::SYN_ACK);

  // Test AND operation
  Flags result = Flags::SYN_ACK & Flags::ACK;
  EXPECT_EQ(static_cast<uint8_t>(result), 0x10);

  // Test XOR operation
  result = Flags::SYN_ACK ^ Flags::SYN;
  EXPECT_EQ(static_cast<uint8_t>(result), 0x10);

  // Test NOT operation
  result = ~Flags::FIN;
  EXPECT_EQ(static_cast<uint8_t>(result), 0xFE);
}

TEST_F(TcpFlagsTest, CompoundAssignment) {
  Flags flags = Flags::SYN;
  flags |= Flags::ACK;
  EXPECT_EQ(flags, Flags::SYN_ACK);

  flags &= Flags::ACK;
  EXPECT_EQ(static_cast<uint8_t>(flags), 0x10);

  flags = Flags::SYN_ACK;
  flags ^= Flags::SYN;
  EXPECT_EQ(static_cast<uint8_t>(flags), 0x10);
}

TEST_F(TcpFlagsTest, FlagChecking) {
  // Test has_flag with TcpFlags
  Flags flags = Flags::SYN_ACK;
  EXPECT_TRUE(has_flag(flags, Flags::SYN));
  EXPECT_TRUE(has_flag(flags, Flags::ACK));
  EXPECT_FALSE(has_flag(flags, Flags::RST));
  EXPECT_FALSE(has_flag(flags, Flags::FIN));

  // Test has_flag with uint8_t
  uint8_t raw_flags = 0x18;  // PSH | ACK
  EXPECT_TRUE(has_flag(raw_flags, Flags::PSH));
  EXPECT_TRUE(has_flag(raw_flags, Flags::ACK));
  EXPECT_FALSE(has_flag(raw_flags, Flags::SYN));
}

TEST_F(TcpFlagsTest, Conversion) {
  // Test to_uint8
  Flags flags = Flags::PSH_ACK;
  uint8_t raw = to_uint8(flags);
  EXPECT_EQ(raw, 0x18);

  // Test from_uint8
  Flags converted = from_uint8(0x12);
  EXPECT_EQ(converted, Flags::SYN_ACK);
}

TEST_F(TcpFlagsTest, MultipleFlagCombinations) {
  // Test combining multiple flags
  Flags flags = Flags::FIN | Flags::PSH | Flags::ACK;
  EXPECT_TRUE(has_flag(flags, Flags::FIN));
  EXPECT_TRUE(has_flag(flags, Flags::PSH));
  EXPECT_TRUE(has_flag(flags, Flags::ACK));
  EXPECT_FALSE(has_flag(flags, Flags::SYN));

  // Remove a flag
  flags &= ~Flags::PSH;
  EXPECT_TRUE(has_flag(flags, Flags::FIN));
  EXPECT_FALSE(has_flag(flags, Flags::PSH));
  EXPECT_TRUE(has_flag(flags, Flags::ACK));
}