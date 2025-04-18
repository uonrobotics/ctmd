#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, expand_dims) {
    constexpr auto x = md::mdarray<double, md::extents<size_t, 2>>{
        {1, 2}, md::extents<size_t, 2>{}};
    const auto b = md::expand_dims<-1>(x);

    constexpr auto b_expect = md::mdarray<double, md::extents<size_t, 2, 1>>{
        {1, 2}, md::extents<size_t, 2, 1>{}};

    ASSERT_TRUE(md::array_equal(b, b_expect));
}
