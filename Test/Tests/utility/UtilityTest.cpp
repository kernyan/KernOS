#include "gtest/gtest.h"
#include <vga.cpp>
#include <utilities.cpp>

TEST(Utilities, Strlen)
{
  const char Str[] = "Hello World";
  EXPECT_EQ(Strlen(Str), 11);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


