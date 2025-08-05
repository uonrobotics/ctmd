#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_array_equal.hpp"
#include "ctmd/math/ctmd_min.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = int;

    static_assert(
        md::array_equal(md::min(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
                            std::array<T, 4>{0, 1, 2, 3}}),
                        0));

    static_assert(md::array_equal(
        md::min<0>(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
            std::array<T, 4>{0, 1, 2, 3}}),
        md::mdarray<T, md::extents<uint8_t, 2>>{std::array<T, 2>{0, 1}}));

    static_assert(md::array_equal(
        md::min<1>(md::mdarray<T, md::extents<uint8_t, 2, 2>>{
            std::array<T, 4>{0, 1, 2, 3}}),
        md::mdarray<T, md::extents<uint8_t, 2>>{std::array<T, 2>{0, 2}}));
}
