#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, matmul) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{5, 6, 7, 8}};
    constexpr auto c = md::linalg::matmul(a, b);

    constexpr auto c_expect = md::mdarray<T, md::extents<size_t, 2, 2>>{
        std::array<T, 4>{19, 22, 43, 50}};

    constexpr bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, matmul) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<2>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<2>{2, 2}};
    const auto b = md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6, 7, 8},
                                               md::dims<2>{2, 2}};
    const auto c = md::linalg::matmul(a, b);

    const auto c_expect = md::mdarray<T, md::dims<2>>{
        std::vector<T>{19, 22, 43, 50}, md::dims<2>{2, 2}};

    const bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}
