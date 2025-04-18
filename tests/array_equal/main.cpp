#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, is_equal) {
    const auto a = md::full<double, md::extents<size_t, 2, 3>>(1);
    const auto b = md::full<double, md::extents<size_t, 2, 3>>(1);
    const bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}

TEST(constexpr, is_equal) {
    constexpr auto a = md::full<double, md::extents<size_t, 2, 3>>(1);
    constexpr auto b = md::full<double, md::extents<size_t, 2, 3>>(1);
    constexpr bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}

TEST(dynamic, is_equal) {
    const auto a = md::full<double, md::dims<2>>(1, md::dims<2>{2, 3});
    const auto b = md::full<double, md::dims<2>>(1, md::dims<2>{2, 3});
    const bool is_equal = md::array_equal(a, b);

    ASSERT_TRUE(is_equal);
}
