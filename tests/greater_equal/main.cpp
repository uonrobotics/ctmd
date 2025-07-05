#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/greater_equal.hpp"

namespace md = ctmd;

TEST(test, 1) {
    using T = double;

    static_assert(md::array_equal(
        md::greater_equal(
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{4, 2, 1}},
            md::mdarray<T, md::extents<size_t, 3>>{std::array<T, 3>{2, 2, 2}}),
        md::mdarray<uint8_t, md::extents<size_t, 3>>{
            std::array<uint8_t, 3>{true, true, false}}));
}
