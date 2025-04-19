#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, copy) {
    using T = double;

    constexpr auto a = md::random::rand<T, md::extents<size_t, 2, 2>>();
    constexpr auto b = md::copy(a);
    constexpr bool array_equal = md::array_equal(a, b);

    ASSERT_TRUE(array_equal);
}

TEST(heap, copy) {
    using T = double;

    const auto a = md::random::rand<T, md::dims<2>>(md::dims<2>{2, 2});
    const auto b = md::copy(a);
    const bool array_equal = md::array_equal(a, b);

    ASSERT_TRUE(array_equal);
}
