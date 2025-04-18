#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, rand) {
    const auto a = md::mdarray<double, md::extents<size_t, 2, 2>>{{1, 2, 3, 4}};
    const auto a_inv = md::linalg::inv(a);

    const auto a_inv_expect =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{-2, 1, 1.5, -0.5}};

    const bool is_allclose = md::allclose(a_inv, a_inv_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(constexpr, rand) {
    constexpr auto a =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{1, 2, 3, 4}};
    constexpr auto a_inv = md::linalg::inv(a);

    constexpr auto a_inv_expect =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{-2, 1, 1.5, -0.5}};

    constexpr bool is_allclose = md::allclose(a_inv, a_inv_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(dynamic, rand) {
    const auto a =
        md::mdarray<double, md::dims<2>>{{1, 2, 3, 4}, md::dims<2>{2, 2}};
    const auto a_inv = md::linalg::inv(a);

    const auto a_inv_expect =
        md::mdarray<double, md::dims<2>>{{-2, 1, 1.5, -0.5}, md::dims<2>{2, 2}};

    const bool is_allclose = md::allclose(a_inv, a_inv_expect);

    ASSERT_TRUE(is_allclose);
}
