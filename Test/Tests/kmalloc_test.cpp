//
// Created by osboxes on 5/26/20.
//

#include "gtest/gtest.h"
#include <memoryallocator.h>

TEST(Utilities, Strlen)
{
    EXPECT_EQ(0x1, 0x1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


