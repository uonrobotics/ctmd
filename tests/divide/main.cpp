#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, divide) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 1>>{std::array<T, 2>{5, 6}};
    constexpr auto c = md::divide(a, b);

    constexpr auto c_expect = md::mdarray<T, md::extents<size_t, 2, 2, 2>>{
        std::array<T, 8>{1. / 5., 2. / 5., 1. / 6., 2. / 6., 3. / 5., 4. / 5.,
                         3. / 6., 4. / 6.}};

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(heap, divide) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::divide(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{1. / 5., 2. / 5., 1. / 6., 2. / 6., 3. / 5., 4. / 5.,
                       3. / 6., 4. / 6.},
        md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(mix, divide) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::divide(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{1. / 5., 2. / 5., 1. / 6., 2. / 6., 3. / 5., 4. / 5.,
                       3. / 6., 4. / 6.},
        md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
