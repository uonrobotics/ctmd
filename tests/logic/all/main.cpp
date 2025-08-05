#include <gtest/gtest.h>

#include "ctmd/logic/ctmd_all.hpp"
#include "ctmd/logic/ctmd_array_equal.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    constexpr auto a = md::mdarray<uint8_t, md::extents<uint8_t, 2, 1, 2>>{
        std::array<uint8_t, 4>{1, 1, 1, 1}};

    constexpr bool b = md::all(a);
    constexpr bool b_expect = true;

    ASSERT_TRUE(b == b_expect);
}

TEST(stack, 2) {
    constexpr auto a = md::mdarray<uint8_t, md::extents<uint8_t, 2, 1, 2>>{
        std::array<uint8_t, 4>{1, 0, 1, 1}};

    constexpr bool b = md::all(a);
    constexpr bool b_expect = false;

    ASSERT_TRUE(b == b_expect);
}

TEST(heap, 1) {
    const auto a = md::mdarray<uint8_t, md::dims<3>>{
        std::vector<uint8_t>{1, 1, 1, 1}, md::dims<3>{2, 1, 2}};

    const bool b = md::all(a);
    const bool b_expect = true;

    ASSERT_TRUE(b == b_expect);
}

TEST(heap, 2) {
    const auto a = md::mdarray<uint8_t, md::dims<3>>{
        std::vector<uint8_t>{1, 0, 1, 1}, md::dims<3>{2, 1, 2}};

    const bool b = md::all(a);
    const bool b_expect = false;

    ASSERT_TRUE(b == b_expect);
}

TEST(test, 1) {
    static_assert(
        md::array_equal(md::all(md::mdarray<int8_t, md::extents<uint8_t, 2, 2>>{
                            std::array<int8_t, 4>{true, false, true, true}}),
                        false));

    static_assert(md::array_equal(
        md::all<0>(md::mdarray<int8_t, md::extents<uint8_t, 2, 2>>{
            std::array<int8_t, 4>{true, false, true, true}}),
        md::mdarray<int8_t, md::extents<uint8_t, 2>>{
            std::array<int8_t, 2>{true, false}}));

    static_assert(
        md::array_equal(md::all(md::mdarray<int8_t, md::extents<uint8_t, 3>>{
                            std::array<int8_t, 3>{-1, 4, 5}}),
                        true));
}
