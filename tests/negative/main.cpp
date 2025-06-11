#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/negative.hpp"

namespace md = ctmd;

TEST(stack, negative) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b = md::negative(a);

    constexpr auto b_expect = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{-1, -2, -3, -4}};

    constexpr bool array_equal = md::array_equal(b, b_expect);

    ASSERT_TRUE(array_equal);
}

TEST(heap, negative) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const auto b = md::negative(a);

    const auto b_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{-1, -2, -3, -4}, md::dims<3>{2, 1, 2}};

    const bool array_equal = md::array_equal(b, b_expect);

    ASSERT_TRUE(array_equal);
}
