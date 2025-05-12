#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, norm) {
    using T = double;

    constexpr auto x =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{3, 4}};
    constexpr auto x_norm = md::linalg::norm(x);

    constexpr auto x_norm_expect = 5;

    constexpr bool allclose = md::allclose(x_norm, x_norm_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, norm) {
    using T = double;

    const auto x =
        md::mdarray<T, md::dims<1>>{std::vector<T>{3, 4}, md::dims<1>{2}};
    const auto x_norm = md::linalg::norm(x);

    const auto x_norm_expect = 5;

    const bool allclose = md::allclose(x_norm, x_norm_expect);

    ASSERT_TRUE(allclose);
}
