#include <gtest/gtest.h>

#include "ctmd/empty_like.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto x = md::empty<T>(md::extents<size_t, 1, 2, 3>{});
    constexpr auto y = md::empty_like(x);

    constexpr auto is_same_extents =
        md::same(y.extents(), md::extents<size_t, 1, 2, 3>{});

    ASSERT_TRUE(is_same_extents);
}

TEST(heap, 2) {
    using T = double;

    const auto x = md::empty<T>(md::dims<3>{1, 2, 3});
    const auto y = md::empty_like(x);

    const auto is_same_extents =
        md::same(y.extents(), md::extents<size_t, 1, 2, 3>{});

    ASSERT_TRUE(is_same_extents);
}
