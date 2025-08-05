#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_allclose.hpp"
#include "ctmd/math/ctmd_sum.hpp"

namespace md = ctmd;

TEST(stack, copy) {
    using T = double;

    constexpr auto in = md::mdarray<T, md::extents<size_t, 2, 3>>{
        std::array<T, 6>{1, 2, 3, 4, 5, 6}};

    static_assert(md::allclose(
        md::sum<-1>(in),
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{6, 15}}));

    static_assert(md::allclose(
        md::sum<0>(in),
        md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{5, 7, 9}}));

    static_assert(md::allclose(
        md::sum<1>(in),
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{6, 15}}));
}

TEST(heap, copy) {
    using T = double;

    const auto in = md::mdarray<T, md::dims<2>>{
        std::vector<T>{1, 2, 3, 4, 5, 6}, md::dims<2>{2, 3}};

    ASSERT_TRUE(md::allclose(
        md::sum<-1>(in),
        md::mdarray<T, md::dims<1>>{std::vector<T>{6, 15}, md::dims<1>{2}}));

    ASSERT_TRUE(md::allclose(
        md::sum<0>(in),
        md::mdarray<T, md::dims<1>>{std::vector<T>{5, 7, 9}, md::dims<1>{3}}));

    ASSERT_TRUE(md::allclose(
        md::sum<1>(in),
        md::mdarray<T, md::dims<1>>{std::vector<T>{6, 15}, md::dims<1>{2}}));
}
