#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, rand) {
    const auto x = md::mdarray<double, md::extents<size_t, 2>>{{3, 4}};
    const auto x_norm = md::linalg::norm(x);

    const auto x_norm_expect =
        md::mdarray<double, md::extents<size_t, 1>>{std::array<double, 1>{5}};

    const bool is_allclose = md::allclose(x_norm, x_norm_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(constexpr, rand) {
    constexpr auto x = md::mdarray<double, md::extents<size_t, 2>>{{3, 4}};
    constexpr auto x_norm = md::linalg::norm(x);

    constexpr auto x_norm_expect =
        md::mdarray<double, md::extents<size_t, 1>>{std::array<double, 1>{5}};

    constexpr bool is_allclose = md::allclose(x_norm, x_norm_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(dynamic, rand) {
    const auto x = md::mdarray<double, md::dims<1>>{{3, 4}, md::dims<1>{2}};
    const auto x_norm = md::linalg::norm(x);

    const auto x_norm_expect = md::mdarray<double, md::dims<1>>{
        std::vector<double>{5}, md::dims<1>{1}};

    const bool is_allclose = md::allclose(x_norm, x_norm_expect);

    ASSERT_TRUE(is_allclose);
}
