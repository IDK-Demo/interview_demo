#include "../../../src/network/tcp/seq_number.h"
#include <gtest/gtest.h>

using namespace idk::net::tcp;

class SeqNumberTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(SeqNumberTest, BasicConstruction) {
  SeqNumber seq1;
  EXPECT_EQ(seq1.value(), 0);
  
  SeqNumber seq2(12345);
  EXPECT_EQ(seq2.value(), 12345);
}

TEST_F(SeqNumberTest, EqualityComparison) {
  SeqNumber seq1(100);
  SeqNumber seq2(100);
  SeqNumber seq3(200);
  
  EXPECT_EQ(seq1, seq2);
  EXPECT_NE(seq1, seq3);
}

TEST_F(SeqNumberTest, LessThanComparison) {
  SeqNumber seq1(100);
  SeqNumber seq2(200);
  
  EXPECT_LT(seq1, seq2);
  EXPECT_FALSE(seq2 < seq1);
}

TEST_F(SeqNumberTest, WrapAroundComparison) {
  // Test wrap-around cases
  SeqNumber seq1(0xFFFFFF00);  // Near the end of 32-bit space
  SeqNumber seq2(0x00000100);  // Near the beginning
  
  // seq2 is "after" seq1 in the circular space (512 bytes ahead)
  EXPECT_LT(seq1, seq2);
  EXPECT_GT(seq2, seq1);
  
  // Test the opposite case
  SeqNumber seq3(0x00000100);
  SeqNumber seq4(0xFFFFFF00);
  
  // seq4 is "before" seq3 in the circular space
  EXPECT_GT(seq3, seq4);
  EXPECT_LT(seq4, seq3);
}

TEST_F(SeqNumberTest, MidpointWrapAround) {
  // Test exactly at the midpoint (2^31 apart)
  SeqNumber seq1(0);
  SeqNumber seq2(0x80000000);  // 2^31
  
  // At exactly 2^31 apart, seq2 is considered "after" seq1
  EXPECT_LT(seq1, seq2);
  
  // Test slightly less than midpoint
  SeqNumber seq3(0);
  SeqNumber seq4(0x7FFFFFFF);  // 2^31 - 1
  
  EXPECT_LT(seq3, seq4);
}

TEST_F(SeqNumberTest, Addition) {
  SeqNumber seq1(100);
  SeqNumber seq2 = seq1 + 50;
  
  EXPECT_EQ(seq2.value(), 150);
  
  // Test wrap-around addition
  SeqNumber seq3(0xFFFFFFF0);
  SeqNumber seq4 = seq3 + 0x20;
  
  EXPECT_EQ(seq4.value(), 0x10);  // Should wrap around
}

TEST_F(SeqNumberTest, AdditionAssignment) {
  SeqNumber seq(100);
  seq += 50;
  
  EXPECT_EQ(seq.value(), 150);
  
  // Test wrap-around
  SeqNumber seq2(0xFFFFFFF0);
  seq2 += 0x20;
  
  EXPECT_EQ(seq2.value(), 0x10);
}

TEST_F(SeqNumberTest, DistanceCalculation) {
  SeqNumber seq1(1000);
  SeqNumber seq2(2000);
  
  EXPECT_EQ(seq2 - seq1, 1000);
  
  // Test wrap-around distance
  SeqNumber seq3(0xFFFFFFF0);
  SeqNumber seq4(0x10);
  
  EXPECT_EQ(seq4 - seq3, 0x20);
}


TEST_F(SeqNumberTest, ThreeWayComparison) {
  SeqNumber seq1(100);
  SeqNumber seq2(200);
  SeqNumber seq3(100);
  
  EXPECT_TRUE((seq1 <=> seq2) < 0);
  EXPECT_TRUE((seq2 <=> seq1) > 0);
  EXPECT_TRUE((seq1 <=> seq3) == 0);
}

TEST_F(SeqNumberTest, RealWorldTcpScenario) {
  // Simulate a real TCP scenario
  SeqNumber initial_seq(1000000);
  SeqNumber current_seq = initial_seq;
  
  // Send some data
  current_seq += 1500;  // MTU-sized packet
  EXPECT_EQ(current_seq.value(), 1001500);
  
  // Receive ACK
  SeqNumber ack(1001500);
  EXPECT_EQ(current_seq, ack);
  
  // Calculate unacknowledged bytes
  SeqNumber last_sent(1002000);
  uint32_t unacked = last_sent - ack;
  EXPECT_EQ(unacked, 500);
}