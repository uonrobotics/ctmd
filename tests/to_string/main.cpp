#include <gtest/gtest.h>

#include "ctmd/to_string.hpp"

namespace md = ctmd;

TEST(stack, to_string) {
    constexpr auto a = md::mdarray<int, md::extents<size_t, 2, 1, 2>>{
        std::array<int, 4>{1, 2, 3, 4}};

    ASSERT_TRUE(md::to_string(a) == "[[[1, 2]], [[3, 4]]]");
    ASSERT_TRUE(md::to_string(a.extents()) == "(2, 1, 2)");
}

TEST(heap, to_string) {
    const auto a = md::mdarray<int, md::dims<3>>{std::vector<int>{1, 2, 3, 4},
                                                 md::dims<3>{2, 1, 2}};

    ASSERT_TRUE(md::to_string(a) == "[[[1, 2]], [[3, 4]]]");
    ASSERT_TRUE(md::to_string(a.extents()) == "(2, 1, 2)");
}
