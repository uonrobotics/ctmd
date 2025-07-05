#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/greater.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = double;

    static_assert(md::array_equal(
        md::greater(
            md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{4, 2}},
            md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{2, 2}}),
        md::mdarray<uint8_t, md::extents<size_t, 2>>{
            std::array<uint8_t, 2>{true, false}}));
}
