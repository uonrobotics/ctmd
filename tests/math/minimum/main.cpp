#include <gtest/gtest.h>

#include "ctmd/creation/ctmd_eye.hpp"
#include "ctmd/logic/ctmd_array_equal.hpp"
#include "ctmd/math/ctmd_minimum.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = double;

    static_assert(md::array_equal(
        md::minimum(
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{2, 3, 4}},
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{1, 5, 2}}),
        md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{1, 3, 2}}));

    static_assert(
        md::array_equal(md::minimum(md::eye<T, md::extents<size_t, 2, 2>>(),
                                    md::mdarray<T, md::extents<size_t, 2>>{
                                        std::array<T, 2>{0.5, 2}}),
                        md::mdarray<T, md::extents<size_t, 2, 2>>{
                            std::array<T, 4>{0.5, 0, 0, 1}}));
}
