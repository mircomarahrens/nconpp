// Copyright 2023 Mirco Marahrens

#ifndef NCONPP_TEST_NCONPP_TESTS_RUN_H_
#define NCONPP_TEST_NCONPP_TESTS_RUN_H_

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    const int result = RUN_ALL_TESTS();

    return result;
}

#endif // NCONPP_TEST_NCONPP_TESTS_RUN_H_
