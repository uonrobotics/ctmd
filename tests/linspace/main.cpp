#include <gtest/gtest.h>

#include "ctmd/full.hpp"
#include "ctmd/linspace.hpp"
#include "ctmd/to_string.hpp"

namespace md = ctmd;

TEST(stack, 1) {
    using T = double;

    constexpr T start = 1;
    constexpr T stop = 10;

    constexpr size_t num = 10;

    constexpr auto out =
        md::linspace<0>(start, stop, md::extents<uint8_t, num>{}, true);

    static_assert(md::same(out.extents(), md::extents<size_t, num>{}));
    static_assert(out(0) == start);
    static_assert(out(num - 1) == stop);
}

TEST(stack, 2) {
    using T = double;

    constexpr T start_val = 1;
    constexpr T stop_val = 10;

    constexpr size_t ext1 = 2;
    constexpr size_t ext2 = 3;
    constexpr size_t num = 10;

    constexpr auto start =
        md::full<T>(start_val, md::extents<size_t, ext1, ext2>{});
    constexpr auto stop =
        md::full<T>(stop_val, md::extents<size_t, ext1, ext2>{});

    constexpr auto out1 =
        md::linspace<0>(start, stop, md::extents<uint8_t, num>{}, true);
    constexpr auto out2 =
        md::linspace<1>(start, stop, md::extents<uint8_t, num>{}, true);
    constexpr auto out3 =
        md::linspace<-1>(start, stop, md::extents<uint8_t, num>{}, true);

    static_assert(
        md::same(out1.extents(), md::extents<size_t, num, ext1, ext2>{}));
    static_assert(out1(0, 0, 0) == start_val);
    static_assert(out1(num - 1, ext1 - 1, ext2 - 1) == stop_val);

    static_assert(
        md::same(out2.extents(), md::extents<size_t, ext1, num, ext2>{}));
    static_assert(out2(0, 0, 0) == start_val);
    static_assert(out2(ext1 - 1, num - 1, ext2 - 1) == stop_val);

    static_assert(
        md::same(out3.extents(), md::extents<size_t, ext1, ext2, num>{}));
    static_assert(out3(0, 0, 0) == start_val);
    static_assert(out3(ext1 - 1, ext2 - 1, num - 1) == stop_val);
}
