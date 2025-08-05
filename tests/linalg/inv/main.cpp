#include <gtest/gtest.h>

#include "ctmd/linalg/ctmd_linalg_inv.hpp"
#include "ctmd/logic/ctmd_allclose.hpp"

namespace md = ctmd;

TEST(stack, inv) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto a_inv = md::linalg::inv(a);

    constexpr auto a_inv_expect = md::mdarray<T, md::extents<size_t, 2, 2>>{
        std::array<T, 4>{-2, 1, 1.5, -0.5}};

    constexpr bool allclose = md::allclose(a_inv, a_inv_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, inv) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<2>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<2>{2, 2}};
    const auto a_inv = md::linalg::inv(a);

    const auto a_inv_expect = md::mdarray<T, md::dims<2>>{
        std::vector<T>{-2, 1, 1.5, -0.5}, md::dims<2>{2, 2}};

    const bool allclose = md::allclose(a_inv, a_inv_expect);

    ASSERT_TRUE(allclose);
}
