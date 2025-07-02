#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/eye.hpp"
#include "ctmd/maximum.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = double;

    static_assert(md::array_equal(
        md::maximum(
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{2, 3, 4}},
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{1, 5, 2}}),
        md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{2, 5, 4}}));

    static_assert(
        md::array_equal(md::maximum(md::eye<T, md::extents<size_t, 2, 2>>(),
                                    md::mdarray<T, md::extents<size_t, 2>>{
                                        std::array<T, 2>{0.5, 2}}),
                        md::mdarray<T, md::extents<size_t, 2, 2>>{
                            std::array<T, 4>{1, 2, 0.5, 2}}));
}
