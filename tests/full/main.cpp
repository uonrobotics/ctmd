#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/full.hpp"

namespace md = ctmd;

TEST(stack, full) {
    using T = double;

    constexpr auto a = md::full<T, md::extents<size_t, 2, 1, 2>>(1);
    constexpr auto a_expect = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 1, 1, 1}};

    constexpr bool array_equal = md::array_equal(a, a_expect);

    ASSERT_TRUE(array_equal);
}

TEST(heap, full) {
    using T = double;

    const auto a = md::full<T, md::dims<3>>(1, md::dims<3>{2, 1, 2});
    const auto a_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{1, 1, 1, 1}, md::dims<3>{2, 1, 2}};

    const bool array_equal = md::array_equal(a, a_expect);

    ASSERT_TRUE(array_equal);
}
