#include <gtest/gtest.h>

#include "ctmd/creation/ctmd_linspace.hpp"
#include "ctmd/logic/ctmd_array_equal.hpp"
#include "ctmd/random/ctmd_random_uniform.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    static_assert(md::array_equal(
        md::linspace((T)2, (T)3, md::extents<uint8_t, 5>{}, true),
        md::mdarray<T, md::extents<size_t, 5>>{
            std::array<T, 5>{2, 2.25, 2.5, 2.75, 3}}));

    static_assert(md::array_equal(
        md::linspace((T)2, (T)3, md::extents<uint8_t, 5>{}, false),
        md::mdarray<T, md::extents<size_t, 5>>{
            std::array<T, 5>{2, 2.2, 2.4, 2.6, 2.8}}));
}

TEST(stack, 2) {
    using T = double;

    constexpr size_t ext1 = 2;
    constexpr size_t ext2 = 3;
    constexpr size_t num = 10;

    constexpr auto start =
        md::random::uniform<T>(md::extents<size_t, ext1, ext2>{}, 0, 1);
    constexpr auto stop =
        md::random::uniform<T>(md::extents<size_t, ext1, ext2>{}, 2, 3);

    constexpr auto out1 =
        md::linspace<0>(start, stop, md::extents<uint8_t, num>{}, true);
    constexpr auto out2 =
        md::linspace<1>(start, stop, md::extents<uint8_t, num>{}, true);
    constexpr auto out3 =
        md::linspace<2>(start, stop, md::extents<uint8_t, num>{}, true);
    constexpr auto out4 =
        md::linspace<-1>(start, stop, md::extents<uint8_t, num>{}, true);

    static_assert(
        md::core::same(out1.extents(), md::extents<size_t, num, ext1, ext2>{}));
    static_assert(out1(0, 0, 0) == start(0, 0));
    static_assert(out1(num - 1, ext1 - 1, ext2 - 1) ==
                  stop(ext1 - 1, ext2 - 1));

    static_assert(
        md::core::same(out2.extents(), md::extents<size_t, ext1, num, ext2>{}));
    static_assert(out2(0, 0, 0) == start(0, 0));
    static_assert(out2(ext1 - 1, num - 1, ext2 - 1) ==
                  stop(ext1 - 1, ext2 - 1));

    static_assert(
        md::core::same(out3.extents(), md::extents<size_t, ext1, ext2, num>{}));
    static_assert(out3(0, 0, 0) == start(0, 0));
    static_assert(out3(ext1 - 1, ext2 - 1, num - 1) ==
                  stop(ext1 - 1, ext2 - 1));

    static_assert(md::array_equal(out3, out4));
}
