#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_array_equal.hpp"
#include "ctmd/manipulation/ctmd_expand_dims.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto x = md::mdarray<T, md::extents<size_t, 2>>{
        std::array<T, 2>{1, 2}, md::extents<size_t, 2>{}};
    const auto b = md::expand_dims<-1>(x);

    constexpr auto b_expect = md::mdarray<T, md::extents<size_t, 2, 1>>{
        std::array<T, 2>{1, 2}, md::extents<size_t, 2, 1>{}};

    const bool array_equal = md::array_equal(b, b_expect);

    ASSERT_TRUE(array_equal);
}

TEST(stack, 2) {
    using T = double;

    constexpr T x = 1;
    const auto b = md::expand_dims<0>(x);

    constexpr auto b_expect = md::mdarray<T, md::extents<size_t, 1>>{
        std::array<T, 1>{1}, md::extents<size_t, 1>{}};

    const bool array_equal = md::array_equal(b, b_expect);

    ASSERT_TRUE(array_equal);
}
