#include <gtest/gtest.h>

#include "ctmd/ctmd_arange.hpp"
#include "ctmd/ctmd_array_equal.hpp"

namespace md = ctmd;

TEST(test, 1) {
    ASSERT_TRUE(md::array_equal(
        md::arange<int>(0, 5, 0.5),
        md::mdarray<int, md::extents<size_t, 10>>{
            std::array<int, 10>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}));

    ASSERT_TRUE(md::array_equal(
        md::arange<int>(-3, 3, 0.5),
        md::mdarray<int, md::extents<size_t, 12>>{
            std::array<int, 12>{-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8}}));

    ASSERT_TRUE(md::array_equal(
        md::arange(3),
        md::mdarray<int, md::extents<size_t, 3>>{std::array<int, 3>{0, 1, 2}}));

    ASSERT_TRUE(md::array_equal(md::arange(3.),
                                md::mdarray<double, md::extents<size_t, 3>>{
                                    std::array<double, 3>{0, 1, 2}}));

    ASSERT_TRUE(md::array_equal(md::arange(3, 7),
                                md::mdarray<double, md::extents<size_t, 4>>{
                                    std::array<double, 4>{3, 4, 5, 6}}));

    ASSERT_TRUE(md::array_equal(md::arange(3, 7, 2),
                                md::mdarray<double, md::extents<size_t, 2>>{
                                    std::array<double, 2>{3, 5}}));
}
