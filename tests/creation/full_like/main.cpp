#include <gtest/gtest.h>

#include "ctmd/creation/ctmd_full_like.hpp"
#include "ctmd/logic/ctmd_array_equal.hpp"

namespace md = ctmd;

TEST(test, 1) {
    constexpr auto x = md::mdarray<int, md::extents<size_t, 6>>{
        std::array<int, 6>{1, 2, 3, 4, 5, 6}};

    static_assert(md::array_equal(md::full_like(x, 1),
                                  md::mdarray<int, md::extents<size_t, 6>>{
                                      std::array<int, 6>{1, 1, 1, 1, 1, 1}}));

    static_assert(md::array_equal(md::full_like(x, 0.1),
                                  md::mdarray<int, md::extents<size_t, 6>>{
                                      std::array<int, 6>{0, 0, 0, 0, 0, 0}}));

    static_assert(md::array_equal(
        md::full_like<double>(x, 0.1),
        md::mdarray<double, md::extents<size_t, 6>>{
            std::array<double, 6>{0.1, 0.1, 0.1, 0.1, 0.1, 0.1}}));

    static_assert(md::array_equal(
        md::full_like<std::optional<double>>(x, std::nullopt),
        md::mdarray<std::optional<double>, md::extents<size_t, 6>>{
            std::array<std::optional<double>, 6>{std::nullopt, std::nullopt,
                                                 std::nullopt, std::nullopt,
                                                 std::nullopt, std::nullopt}}));
}
