#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, expand_dims) {
    // NOTE: mdspan does not support constexpr

    using T = double;

    constexpr auto x = md::mdarray<T, md::extents<size_t, 2>>{
        std::array<T, 2>{1, 2}, md::extents<size_t, 2>{}};
    const auto b = md::expand_dims<-1>(x);

    constexpr auto b_expect = md::mdarray<T, md::extents<size_t, 2, 1>>{
        std::array<T, 2>{1, 2}, md::extents<size_t, 2, 1>{}};

    const bool array_equal = md::array_equal(b, b_expect);

    ASSERT_TRUE(array_equal);
}
