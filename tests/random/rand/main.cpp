#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, rand) {
    const auto a = md::random::rand<double, md::extents<size_t, 2, 2>>();
    std::cout << "a: " << md::to_string(a) << std::endl;

    ASSERT_TRUE(true);
}

TEST(constexpr, rand) {
    constexpr auto a = md::random::rand<double, md::extents<size_t, 2, 2>>();
    std::cout << "a: " << md::to_string(a) << std::endl;

    ASSERT_TRUE(true);
}

TEST(dynamic, rand) {
    const auto a = md::random::rand<double, md::dims<2>>(md::dims<2>{2, 2});
    std::cout << "a: " << md::to_string(a) << std::endl;

    ASSERT_TRUE(true);
}
