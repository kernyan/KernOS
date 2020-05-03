#include "gtest/gtest.h"
#include <vga.cpp>
#include <utilities.cpp>

TEST(Display, VGAPrint)
{
  EXPECT_EQ(1, 1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


