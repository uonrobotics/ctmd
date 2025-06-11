#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/eye.hpp"

namespace md = ctmd;

TEST(stack, fill) {
    using T = double;

    constexpr auto out = md::eye<T>(md::extents<size_t, 2, 3, 2>{});

    constexpr bool is_array_equal = md::array_equal(
        out, md::mdarray<T, md::extents<size_t, 2, 3, 2>>{
                 std::array<T, 12>{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}});

    ASSERT_TRUE(is_array_equal);
}

TEST(heap, fill) {
    using T = double;

    const auto out = md::eye<T>(md::dims<3>{2, 3, 2});

    const bool is_array_equal = md::array_equal(
        out, md::mdarray<T, md::extents<size_t, 2, 3, 2>>{
                 std::array<T, 12>{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}});

    ASSERT_TRUE(is_array_equal);
}
