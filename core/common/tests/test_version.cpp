/// @file test_version.cpp
/// @brief Unit tests for UADOS version information.

#include "uados/version.hpp"

#include <gtest/gtest.h>

namespace uados::test {

TEST(VersionTest, CurrentVersionIsValid) {
    EXPECT_EQ(kVersion.major, 0);
    EXPECT_EQ(kVersion.minor, 1);
    EXPECT_EQ(kVersion.patch, 0);
}

TEST(VersionTest, VersionStringMatchesComponents) {
    EXPECT_EQ(kVersionString, "0.1.0");
}

TEST(VersionTest, ProjectNameIsCorrect) {
    EXPECT_EQ(kProjectName, "UADOS");
}

TEST(VersionTest, VersionComparison) {
    Version v1{0, 1, 0};
    Version v2{0, 1, 0};
    Version v3{0, 2, 0};
    Version v4{1, 0, 0};

    EXPECT_EQ(v1, v2);
    EXPECT_LT(v1, v3);
    EXPECT_LT(v3, v4);
    EXPECT_GT(v4, v1);
}

TEST(VersionTest, VersionOrdering) {
    Version versions[] = {
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
    };

    for (size_t i = 0; i < std::size(versions) - 1; ++i) {
        EXPECT_LT(versions[i], versions[i + 1])
            << "versions[" << i << "] should be less than versions[" << i + 1 << "]";
    }
}

} // namespace uados::test
