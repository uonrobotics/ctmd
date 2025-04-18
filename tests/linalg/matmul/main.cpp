#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, rand) {
    const auto a = md::mdarray<double, md::extents<size_t, 2, 2>>{{1, 2, 3, 4}};
    const auto b = md::mdarray<double, md::extents<size_t, 2, 2>>{{5, 6, 7, 8}};
    const auto c = md::linalg::matmul(a, b);

    const auto c_expect =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{19, 22, 43, 50}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(constexpr, rand) {
    constexpr auto a =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{5, 6, 7, 8}};
    constexpr auto c = md::linalg::matmul(a, b);

    constexpr auto c_expect =
        md::mdarray<double, md::extents<size_t, 2, 2>>{{19, 22, 43, 50}};

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(dynamic, rand) {
    const auto a =
        md::mdarray<double, md::dims<2>>{{1, 2, 3, 4}, md::dims<2>{2, 2}};
    const auto b =
        md::mdarray<double, md::dims<2>>{{5, 6, 7, 8}, md::dims<2>{2, 2}};
    const auto c = md::linalg::matmul(a, b);

    const auto c_expect =
        md::mdarray<double, md::dims<2>>{{19, 22, 43, 50}, md::dims<2>{2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
