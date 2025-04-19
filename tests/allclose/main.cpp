#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, allclose) {
    using T = double;
    constexpr T scale = 1e-5;

    constexpr auto a = md::random::rand<T, md::extents<size_t, 2, 3>>();
    constexpr auto a_diff = md::multiply(
        md::random::rand<T, md::extents<size_t, 2, 3>>(),
        md::mdarray<T, md::extents<size_t, 1>>{std::array<T, 1>{scale}});

    constexpr auto b = md::add(a, a_diff);

    constexpr bool allclose = md::allclose(a, b, 0, scale);

    ASSERT_TRUE(allclose);
}

TEST(heap, is_equal) {
    using T = double;
    const T scale = 1e-5;

    const auto a = md::random::rand<T, md::dims<2>>(md::dims<2>{2, 3});
    const auto a_diff = md::multiply(
        md::random::rand<T, md::dims<2>>(md::dims<2>{2, 3}),
        md::mdarray<T, md::dims<1>>{std::vector<T>{scale}, md::dims<1>{1}});

    const auto b = md::add(a, a_diff);

    const bool allclose = md::allclose(a, b, 0, scale);

    ASSERT_TRUE(allclose);
}
