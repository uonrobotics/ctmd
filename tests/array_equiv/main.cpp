#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, array_equiv) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{1, 2}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{1, 2, 1, 2}};
    constexpr auto is_array_equiv = md::array_equiv(a, b);

    ASSERT_TRUE(is_array_equiv);
}

TEST(heap, array_equiv) {
    using T = double;

    const auto a =
        md::mdarray<T, md::dims<1>>{std::vector<T>{1, 2}, md::dims<1>{2}};
    const auto b = md::mdarray<T, md::dims<2>>{std::vector<T>{1, 2, 1, 2},
                                               md::dims<2>{2, 2}};
    const auto is_array_equiv = md::array_equiv(a, b);

    ASSERT_TRUE(is_array_equiv);
}
