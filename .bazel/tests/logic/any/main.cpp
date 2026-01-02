#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_any.hpp"
#include "ctmd/logic/ctmd_array_equal.hpp"

namespace md = ctmd;

TEST(test, 1) {
    static_assert(
        md::array_equal(md::any(md::mdarray<int8_t, md::extents<uint8_t, 2, 2>>{
                            std::array<int8_t, 4>{true, false, true, false}}),
                        true));

    static_assert(md::array_equal(
        md::any<0>(md::mdarray<int8_t, md::extents<uint8_t, 2, 3>>{
            std::array<int8_t, 6>{true, false, true, false, false, false}}),
        md::mdarray<int8_t, md::extents<uint8_t, 3>>{
            std::array<int8_t, 3>{true, false, true}}));

    static_assert(
        md::array_equal(md::any(md::mdarray<int8_t, md::extents<uint8_t, 3>>{
                            std::array<int8_t, 3>{-1, 0, 5}}),
                        true));
}

TEST(test, 2) {
    using T = int8_t;

    constexpr auto a = md::mdarray<T, md::extents<uint8_t, 3, 3>>{
        std::array<T, 9>{1, 0, 0, 0, 0, 1, 0, 0, 0}};

    static_assert(md::array_equal(md::any<0>(a),
                                  md::mdarray<T, md::extents<uint8_t, 3>>{
                                      std::array<T, 3>{true, false, true}}));

    static_assert(md::array_equal(md::any<1>(a),
                                  md::mdarray<T, md::extents<uint8_t, 3>>{
                                      std::array<T, 3>{true, true, false}}));
}
