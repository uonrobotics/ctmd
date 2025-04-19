#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, multiply) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 1>>{std::array<T, 2>{5, 6}};
    constexpr auto c = md::multiply(a, b);

    constexpr auto c_expect = md::mdarray<T, md::extents<size_t, 2, 2, 2>>{
        std::array<T, 8>{5, 10, 6, 12, 15, 20, 18, 24}};

    std::cout << md::to_string(c) << std::endl;
    std::cout << md::to_string(c_expect) << std::endl;

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(heap, multiply) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::multiply(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{5, 10, 6, 12, 15, 20, 18, 24}, md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(mix, multiply) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::multiply(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{5, 10, 6, 12, 15, 20, 18, 24}, md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
