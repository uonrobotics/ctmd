#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(const, add) {
    const auto a = md::full<double, md::extents<size_t, 3, 1, 2>>(1);
    const auto b = md::full<double, md::extents<size_t, 2, 1>>(2);
    const auto c = md::add(a, b);

    const auto c_expect = md::full<double, md::extents<size_t, 3, 2, 2>>(3);

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(constexpr, add) {
    constexpr auto a = md::full<int, md::extents<size_t, 3, 1, 2>>(1);
    constexpr auto b = md::full<int, md::extents<size_t, 2, 1>>(2);
    constexpr auto c = md::add(a, b);

    constexpr auto c_expect = md::full<int, md::extents<size_t, 3, 2, 2>>(3);

    constexpr bool is_allclose = md::allclose(c, c_expect);

    // std::cout << "a extents: " << md::to_string(a.extents()) << std::endl;
    // std::cout << "a: " << md::to_string(a) << std::endl;
    // std::cout << "b extents: " << md::to_string(b.extents()) << std::endl;
    // std::cout << "b: " << md::to_string(b) << std::endl;
    // std::cout << "c extents: " << md::to_string(c.extents()) << std::endl;
    // std::cout << "c: " << md::to_string(c) << std::endl;

    ASSERT_TRUE(is_allclose);
}

TEST(dynamic, add) {
    const auto a = md::full<double, md::dims<3>>(1, md::dims<3>{3, 1, 2});
    const auto b = md::full<double, md::dims<2>>(2, md::dims<2>{2, 1});
    const auto c = md::add(a, b);

    const auto c_expect =
        md::full<double, md::dims<3>>(3, md::dims<3>{3, 2, 2});

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}

TEST(mix, add) {
    constexpr auto a = md::full<double, md::extents<size_t, 3, 1, 2>>(1);
    const auto b = md::full<double, md::dims<2>>(2, md::dims<2>{2, 1});
    const auto c = md::add(a, b);

    const auto c_expect =
        md::full<double, md::dims<3>>(3, md::dims<3>{3, 2, 2});

    const bool is_allclose = md::allclose(c, c_expect);

    ASSERT_TRUE(is_allclose);
}
