#include <gtest/gtest.h>

#include "ctmd/ctmd_allclose.hpp"
#include "ctmd/ctmd_rad2deg.hpp"

namespace md = ctmd;

TEST(single, deg2rad) {
    using T = double;

    constexpr T a = M_PI_2;
    constexpr T b = md::rad2deg(a);

    constexpr T b_expect = 90;

    constexpr bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}

TEST(stack, rad2deg) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 3>>{
        std::array<T, 3>{0, M_PI_2, M_PI}};
    constexpr auto b = md::rad2deg(a);

    constexpr auto b_expect =
        md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{0, 90, 180}};

    constexpr bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, rad2deg) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<1>>{std::vector<T>{0, M_PI_2, M_PI},
                                               md::dims<1>{3}};
    const auto b = md::rad2deg(a);

    const auto b_expect =
        md::mdarray<T, md::dims<1>>{std::vector<T>{0, 90, 180}, md::dims<1>{3}};

    const bool allclose = md::allclose(b, b_expect);

    ASSERT_TRUE(allclose);
}
