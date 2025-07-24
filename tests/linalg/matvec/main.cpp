#include <gtest/gtest.h>

#include "ctmd/ctmd_allclose.hpp"
#include "ctmd/linalg/ctmd_linalg_matvec.hpp"

namespace md = ctmd;

TEST(stack, matvec) {
    using T = double;

    constexpr auto a =
        md::mdarray<T, md::extents<size_t, 2, 2>>{std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{5, 6}};
    constexpr auto c = md::linalg::matvec(a, b);

    constexpr auto c_expect =
        md::mdarray<T, md::extents<size_t, 2>>{std::array<T, 2>{17, 39}};

    constexpr bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}

TEST(heap, matvec) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<2>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<2>{2, 2}};
    const auto b =
        md::mdarray<T, md::dims<1>>{std::vector<T>{5, 6}, md::dims<1>{2}};
    const auto c = md::linalg::matvec(a, b);

    const auto c_expect =
        md::mdarray<T, md::dims<1>>{std::vector<T>{17, 39}, md::dims<1>{2}};

    const bool allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(allclose);
}
