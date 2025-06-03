#include <gtest/gtest.h>

#include "ctmd/ctmd_allclose.hpp"
#include "ctmd/ctmd_sin.hpp"

namespace md = ctmd;

#ifdef REAL_GCC

TEST(stack, negative) {
    using T = float;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 4>>{
        std::array<T, 4>{0, M_PI_4, M_PI_2, M_PI}};
    constexpr auto b = md::sin(a);

    constexpr bool is_allclose =
        md::allclose(b,
                     md::mdarray<T, md::extents<size_t, 4>>{std::array<T, 4>{
                         0.000000, 0.707107, 1.000000, -0.000000}},
                     0, 1e-5);

    ASSERT_TRUE(is_allclose);
}

#endif

TEST(heap, negative) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<1>>{
        std::vector<T>{0, M_PI_4, M_PI_2, M_PI}, md::dims<1>{4}};
    const auto b = md::sin(a);

    const bool is_allclose =
        md::allclose(b,
                     md::mdarray<T, md::extents<size_t, 4>>{std::array<T, 4>{
                         0.000000, 0.707107, 1.000000, -0.000000}},
                     0, 1e-5);

    ASSERT_TRUE(is_allclose);
}
