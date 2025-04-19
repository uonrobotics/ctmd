#include <gtest/gtest.h>

#include "ctmd/ctmd.hpp"

namespace md = ctmd;

TEST(stack, rand) {
    using T = double;

    constexpr auto a = md::random::rand<T, md::extents<size_t, 2, 2>>();

    std::cout << md::to_string(a) << std::endl;

    ASSERT_TRUE(!md::allclose(a, md::full<T, md::extents<size_t, 2, 2>>(0)));
}

TEST(heap, rand) {
    using T = double;

    const auto a = md::random::rand<T, md::dims<2>>(md::dims<2>{2, 2});

    std::cout << md::to_string(a) << std::endl;

    ASSERT_TRUE(!md::allclose(a, md::full<T, md::extents<size_t, 2, 2>>(0)));
}
