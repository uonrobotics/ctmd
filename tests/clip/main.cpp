#include <gtest/gtest.h>

#include "ctmd/ctmd_clip.hpp"
#include "ctmd/random/ctmd_random_rand.hpp"

namespace md = ctmd;

TEST(stack, clip) {
    using T = double;

    constexpr auto a = md::random::rand<T, md::extents<size_t, 2, 1, 2>>();
    constexpr auto a_min =
        md::mdarray<T, md::extents<size_t, 1>>{std::array<T, 1>{0.4}};
    constexpr auto a_max =
        md::mdarray<T, md::extents<size_t, 1>>{std::array<T, 1>{0.6}};

    constexpr auto a_clip = md::clip(a, a_min, a_max);

    for (size_t i = 0; i < a.extent(0); i++) {
        for (size_t j = 0; j < a.extent(1); j++) {
            for (size_t k = 0; k < a.extent(2); k++) {
                ASSERT_TRUE((a_min(0) <= a_clip(i, j, k) &&
                             a_clip(i, j, k) <= a_max(0)));
            }
        }
    }
}

TEST(stack, clip_scalar) {
    using T = double;

    constexpr auto a = md::random::rand<T, md::extents<size_t, 2, 1, 2>>();
    constexpr T a_min = 0.4;
    constexpr T a_max = 0.6;

    constexpr auto a_clip = md::clip(a, a_min, a_max);

    for (size_t i = 0; i < a.extent(0); i++) {
        for (size_t j = 0; j < a.extent(1); j++) {
            for (size_t k = 0; k < a.extent(2); k++) {
                ASSERT_TRUE(
                    (a_min <= a_clip(i, j, k) && a_clip(i, j, k) <= a_max));
            }
        }
    }
}

TEST(heap, clip) {
    using T = double;

    const auto a = md::random::rand<T, md::dims<3>>(md::dims<3>{2, 1, 2});
    const auto a_min =
        md::mdarray<T, md::dims<1>>{std::vector<T>{0.4}, md::dims<1>{1}};
    const auto a_max =
        md::mdarray<T, md::dims<1>>{std::vector<T>{0.6}, md::dims<1>{1}};

    const auto a_clip = md::clip(a, a_min, a_max);

    for (size_t i = 0; i < a.extent(0); i++) {
        for (size_t j = 0; j < a.extent(1); j++) {
            for (size_t k = 0; k < a.extent(2); k++) {
                ASSERT_TRUE((a_min(0) <= a_clip(i, j, k) &&
                             a_clip(i, j, k) <= a_max(0)));
            }
        }
    }
}

TEST(heap, clip_scalar) {
    using T = double;

    const auto a = md::random::rand<T, md::dims<3>>(md::dims<3>{2, 1, 2});
    const T a_min = 0.4;
    const T a_max = 0.6;

    const auto a_clip = md::clip(a, a_min, a_max);

    for (size_t i = 0; i < a.extent(0); i++) {
        for (size_t j = 0; j < a.extent(1); j++) {
            for (size_t k = 0; k < a.extent(2); k++) {
                ASSERT_TRUE(
                    (a_min <= a_clip(i, j, k) && a_clip(i, j, k) <= a_max));
            }
        }
    }
}
