#include <gtest/gtest.h>

#include "ctmd/ctmd_array_equiv.hpp"
#include "ctmd/ctmd_to_string.hpp"
#include "ctmd/random/ctmd_random_rand.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto out = md::random::rand<T, md::extents<size_t, 2, 2>>();

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(stack, 2) {
    using T = double;

    constexpr auto out = md::random::rand<T>();

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(heap, 1) {
    using T = double;

    const auto out = md::random::rand<T, md::dims<2>>(md::dims<2>{2, 2});

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(heap, 2) {
    using T = double;

    const auto out = md::random::rand<T>();

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}
