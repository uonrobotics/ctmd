#include <gtest/gtest.h>

#include "ctmd/allclose.hpp"
#include "ctmd/deg2rad.hpp"

namespace md = ctmd;

TEST(single, deg2rad) {
    using T = double;

    constexpr T a = 90;
    constexpr T b = md::deg2rad(a);

    constexpr T b_expect = M_PI_2;

    constexpr bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}

TEST(stack, deg2rad) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{0, 90, 180}};
    constexpr auto b = md::deg2rad(a);

    constexpr auto b_expect = md::mdarray<T, md::extents<size_t, 3>>{
        std::array<T, 3>{0, M_PI_2, M_PI}};

    constexpr bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, deg2rad) {
    using T = double;

    const auto a =
        md::mdarray<T, md::dims<1>>{std::vector<T>{0, 90, 180}, md::dims<1>{3}};
    const auto b = md::deg2rad(a);

    const auto b_expect = md::mdarray<T, md::dims<1>>{
        std::vector<T>{0, M_PI_2, M_PI}, md::dims<1>{3}};

    const bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}
