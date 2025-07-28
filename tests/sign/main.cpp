#include <gtest/gtest.h>

#include "ctmd/ctmd_array_equal.hpp"
#include "ctmd/ctmd_sign.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = double;

    static_assert(
        md::array_equal(md::sign(md::mdarray<T, md::extents<size_t, 2>>{
                            std::array<T, 2>{-5, 4.5}}),
                        md::mdarray<int8_t, md::extents<size_t, 2>>{
                            std::array<int8_t, 2>{-1, 1}}));

    static_assert(md::array_equal(md::sign(0), 0));

    static_assert(md::array_equal(md::sign(2), 1));
    static_assert(md::array_equal(md::sign(-3), -1));
}
