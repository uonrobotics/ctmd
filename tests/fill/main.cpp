#include <gtest/gtest.h>

#include "ctmd/array_equal.hpp"
#include "ctmd/fill.hpp"

namespace md = ctmd;

TEST(stack, fill) {
    using T = double;

    auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{};
    md::fill(a, 1);

    constexpr auto a_expect = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 1, 1, 1}};

    const bool array_equal = md::array_equal(a, a_expect);

    ASSERT_TRUE(array_equal);
}

TEST(heap, fill) {
    using T = double;

    auto a = md::mdarray<T, md::dims<3>>{md::dims<3>{2, 1, 2}};
    md::fill(a, 1);

    const auto a_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{1, 1, 1, 1}, md::dims<3>{2, 1, 2}};

    const bool array_equal = md::array_equal(a, a_expect);

    ASSERT_TRUE(array_equal);
}
