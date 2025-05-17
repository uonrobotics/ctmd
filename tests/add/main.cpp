#include <gtest/gtest.h>

#include "ctmd/ctmd_add.hpp"
#include "ctmd/ctmd_allclose.hpp"
#include "ctmd/ctmd_to_string.hpp"

namespace md = ctmd;

TEST(stack, add) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr auto b =
        md::mdarray<T, md::extents<size_t, 2, 1>>{std::array<T, 2>{5, 6}};
    constexpr auto c = md::add(a, b);

    constexpr auto c_expect = md::mdarray<T, md::extents<size_t, 2, 2, 2>>{
        std::array<T, 8>{6, 7, 7, 8, 8, 9, 9, 10}};

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(stack, add_scalar) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    constexpr T b = 5;
    constexpr auto c = md::add(a, b);

    constexpr auto c_expect = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{6, 7, 8, 9}};

    constexpr bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(heap, add) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::add(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{6, 7, 7, 8, 8, 9, 9, 10}, md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(heap, add_scalar) {
    using T = double;

    const auto a = md::mdarray<T, md::dims<3>>{std::vector<T>{1, 2, 3, 4},
                                               md::dims<3>{2, 1, 2}};
    const T b = 5;
    const auto c = md::add(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{6, 7, 8, 9}, md::dims<3>{2, 1, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(mix, add) {
    using T = double;

    constexpr auto a = md::mdarray<T, md::extents<size_t, 2, 1, 2>>{
        std::array<T, 4>{1, 2, 3, 4}};
    const auto b =
        md::mdarray<T, md::dims<2>>{std::vector<T>{5, 6}, md::dims<2>{2, 1}};
    const auto c = md::add(a, b);

    const auto c_expect = md::mdarray<T, md::dims<3>>{
        std::vector<T>{6, 7, 7, 8, 8, 9, 9, 10}, md::dims<3>{2, 2, 2}};

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
