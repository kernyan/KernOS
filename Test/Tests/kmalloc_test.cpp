//
// Created by osboxes on 5/26/20.
//

#include "gtest/gtest.h"
#include <memoryallocator.h>

TEST(kmalloc, AllocateFree)
{
    uint8_t Array[4096*3];

    const uint32_t StartAdd = (uint32_t)  Array;
    const uint32_t EndAdd   = (uint32_t) (Array + 4096*3);

    KM::mem_alloc.Initialize (StartAdd, EndAdd);
    EXPECT_EQ(KM::mem_alloc.m_Base.m_Next->m_Size, EndAdd - StartAdd);

    int *Int1       = (int*) KM::kmalloc(sizeof(int));
    *Int1           = 65537;
    double* Double1 = (double*) KM::kmalloc((sizeof(double)));
    *Double1        = 1.23456789;

    EXPECT_EQ(*Int1, 65537);
    EXPECT_EQ(*Double1, 1.23456789);
    EXPECT_TRUE(((uint32_t) Int1 < EndAdd) && ((uint32_t) Int1 > StartAdd));
    EXPECT_TRUE(((uint32_t) Double1 < EndAdd) && ((uint32_t) Double1 > StartAdd));

    EXPECT_EQ(KM::mem_alloc.m_Base.m_Next->m_Size, EndAdd - StartAdd - sizeof(int) - sizeof(double) - sizeof(KM::Header) * 2);

    KM::free(Int1);

    EXPECT_EQ(KM::mem_alloc.m_Base.m_Next->m_Size, sizeof(int) + sizeof(KM::Header));

    KM::free(Double1);

    EXPECT_EQ(KM::mem_alloc.m_Base.m_Next->m_Size, EndAdd - StartAdd);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


