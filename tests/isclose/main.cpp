#include <gtest/gtest.h>

#include "ctmd/ctmd_array_equal.hpp"
#include "ctmd/ctmd_isclose.hpp"

namespace md = ctmd;

TEST(stack, isclose) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 1>>{std::array<T, 2>{2, 3}};
    constexpr auto c = md::isclose(a, b, 0, 1);

    constexpr auto c_expect =
        md::mdarray<uint8_t, md::extents<size_t, 2, 2, 2>>{
            std::array<uint8_t, 8>{1, 1, 0, 1, 1, 0, 1, 1}};

    constexpr auto is_array_equal = md::array_equal(c, c_expect);

    ASSERT_TRUE(is_array_equal);
}

TEST(heap, isclose) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{2, 3}, md::dims<2>{2, 1}};
    const auto c = md::isclose(a, b, 0, 1);

    const auto c_expect = md::mdarray<uint8_t, md::dims<3>>{
        std::vector<uint8_t>{1, 1, 0, 1, 1, 0, 1, 1}, md::dims<3>{2, 2, 2}};

    const auto is_array_equal = md::array_equal(c, c_expect);

    ASSERT_TRUE(is_array_equal);
}
