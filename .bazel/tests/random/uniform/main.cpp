#include <gtest/gtest.h>

#include "ctmd/ctmd_to_string.hpp"
#include "ctmd/logic/ctmd_array_equiv.hpp"
#include "ctmd/random/ctmd_random_uniform.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr auto out =
        md::random::uniform<T>(md::extents<size_t, 2, 2>{}, -1, 1);

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(stack, 2) {
    using T = double;

    constexpr auto out =
        md::random::uniform<T>(md::extents<size_t, 1>{}, -1, 1);

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(heap, 1) {
    using T = double;

    const auto out = md::random::uniform<T>(md::dims<2>{2, 2}, -1, 1);

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}

TEST(heap, 2) {
    using T = double;

    const auto out = md::random::uniform<T>(md::dims<1>{1}, -1, 1);

    std::cout << md::to_string(out) << std::endl;

    ASSERT_TRUE(!md::array_equiv(out, 0));
}
