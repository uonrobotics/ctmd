#include <gtest/gtest.h>

#include "ctmd/linalg/ctmd_linalg_vecmat.hpp"
#include "ctmd/logic/ctmd_allclose.hpp"

namespace md = ctmd;

TEST(stack, vecmat) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{1, 2}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{5, 6, 7, 8}};
    constexpr auto c = md::linalg::vecmat(a, b);

    constexpr auto c_expect =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{19, 22}};

    constexpr bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, vecmat) {
    using T = double;

    const auto a =
        md::mdarray<T, md::dims<1>>{std::vector<T>{1, 2}, md::dims<1>{2}};
    const auto b = md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6, 7, 8},
                                               md::dims<2>{2, 2}};
    const auto c = md::linalg::vecmat(a, b);

    const auto c_expect =
        md::mdarray<T, md::dims<1>>{std::vector<T>{19, 22}, md::dims<1>{2}};

    const bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}
