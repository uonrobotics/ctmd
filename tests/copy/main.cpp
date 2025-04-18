#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, copy) {
    const auto a = md::random::rand<double, md::extents<size_t, 2, 2>>();
    const auto b = md::copy(a);
    const bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}

TEST(constexpr, copy) {
    constexpr auto a = md::random::rand<double, md::extents<size_t, 2, 2>>();
    constexpr auto b = md::copy(a);
    constexpr bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}

TEST(dynamic, copy) {
    const auto a = md::random::rand<double, md::dims<2>>(md::dims<2>{2, 2});
    const auto b = md::copy(a);
    const bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}
