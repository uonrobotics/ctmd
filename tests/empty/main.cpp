#include <gtest/gtest.h>

#include "ctmd/empty.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto x = md::empty<T>(md::extents<size_t, 1, 2, 3>{});

    constexpr auto is_same_extents =
        md::core::same(x.extents(), md::extents<size_t, 1, 2, 3>{});

    ASSERT_TRUE(is_same_extents);
}

TEST(heap, 2) {
    using T = double;

    const auto x = md::empty<T>(md::dims<3>{1, 2, 3});

    const auto is_same_extents =
        md::core::same(x.extents(), md::extents<size_t, 1, 2, 3>{});

    ASSERT_TRUE(is_same_extents);
}
