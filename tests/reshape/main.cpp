#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto in = md::mdarray<T, md::extents<size_t, 2, 3>>{
        std::array<T, 6>{1, 2, 3, 4, 5, 6}};

    constexpr bool array_equal =
        md::array_equal(md::reshape(in, md::extents<size_t, 6>{}),
                        md::mdarray<T, md::extents<size_t, 6>>{
                            std::array<T, 6>{1, 2, 3, 4, 5, 6}});

    ASSERT_TRUE(array_equal);
}

TEST(stack, 2) {
    using T = double;

    const auto in = md::mdarray<T, md::dims<2>>{
        std::vector<T>{1, 2, 3, 4, 5, 6}, md::dims<2>{2, 3}};

    const bool array_equal =
        md::array_equal(md::reshape(in, md::extents<size_t, 6>{}),
                        md::mdarray<T, md::dims<1>>{
                            std::vector<T>{1, 2, 3, 4, 5, 6}, md::dims<1>{6}});

    ASSERT_TRUE(array_equal);
}
