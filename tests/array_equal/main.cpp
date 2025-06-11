#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"

namespace md = ctmd;

TEST(stack, array_equal) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr bool array_equal = md::array_equal(a, b);

    ASSERT_TRUE(array_equal);
}

TEST(heap, array_equal) {
    using T = double;

    const auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const auto b = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const bool array_equal = md::array_equal(a, b);

    ASSERT_TRUE(array_equal);
}

TEST(mix, array_equal) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const auto b = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const bool array_equal = md::array_equal(a, b);

    ASSERT_TRUE(array_equal);
}
