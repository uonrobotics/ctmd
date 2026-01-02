#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_array_equal.hpp"
#include "ctmd/manipulation/ctmd_concatenate.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 1, 2>>{std::array<T, 2>{5, 6}};

    constexpr auto out = md::concatenate<0>(std::forward_as_tuple(a, b));

    constexpr bool is_array_equal =
        md::array_equal(out, md::mdarray<T, md::extents<size_t, 3, 2>>{
                                 std::array<T, 6>{1, 2, 3, 4, 5, 6}});

    ASSERT_TRUE(is_array_equal);
}

TEST(stack, 2) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<2>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<2>{2, 2}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{1, 2}};

    const auto out = md::concatenate<0>(std::forward_as_tuple(a, b));

    const bool is_array_equal =
        md::array_equal(out, md::mdarray<T, md::extents<size_t, 3, 2>>{
                                 std::array<T, 6>{1, 2, 3, 4, 5, 6}});

    ASSERT_TRUE(is_array_equal);
}
