#include <gtest/gtest.h>

#include "ctmd/ctmd_array_equal.hpp"
#include "ctmd/ctmd_max.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = int;

    static_assert(
        md::array_equal(md::max(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
                            std::array<T, 4>{0, 1, 2, 3}}),
                        3));

    static_assert(md::array_equal(
        md::max<0>(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
            std::array<T, 4>{0, 1, 2, 3}}),
        md::mdarray<T, md::extents<uint8_t, 2>>{std::array<T, 2>{2, 3}}));

    static_assert(md::array_equal(
        md::max<1>(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
            std::array<T, 4>{0, 1, 2, 3}}),
        md::mdarray<T, md::extents<uint8_t, 2>>{std::array<T, 2>{1, 3}}));
}
