#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, divide) {
    const auto a = md::full<double, md::extents<size_t, 3, 1, 2>>(6);
    const auto b = md::full<double, md::extents<size_t, 2, 1>>(2);
    const auto c = md::divide(a, b);

    const auto c_expect = md::full<double, md::extents<size_t, 3, 2, 2>>(3);

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(constexpr, divide) {
    constexpr auto a = md::full<double, md::extents<size_t, 3, 1, 2>>(6);
    constexpr auto b = md::full<double, md::extents<size_t, 2, 1>>(2);
    constexpr auto c = md::divide(a, b);

    constexpr auto c_expect = md::full<double, md::extents<size_t, 3, 2, 2>>(3);

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(dynamic, divide) {
    const auto a = md::full<double, md::dims<3>>(6, md::dims<3>{3, 1, 2});
    const auto b = md::full<double, md::dims<2>>(2, md::dims<2>{2, 1});
    const auto c = md::divide(a, b);

    const auto c_expect =
        md::full<double, md::dims<3>>(3, md::dims<3>{3, 2, 2});

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
