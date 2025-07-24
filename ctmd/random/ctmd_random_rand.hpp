#pragma once

#include <cstdint>
#include <random>

#include "../ctmd_copy.hpp"
#include "../ctmd_empty.hpp"

namespace ctmd {
namespace random {
namespace detail {

// Code adapted from:
// https://mklimenko.github.io/english/2018/06/04/constexpr-random/

constexpr std::uint32_t lce_a = 16807;
constexpr std::uint32_t lce_c = 0;
constexpr std::uint32_t lce_m = 2147483647;

[[nodiscard]] inline constexpr std::uint32_t time_from_string(const char *str,
                                                              int offset) {
    return static_cast<std::uint32_t>(str[offset] - '0') * 10 +
           static_cast<std::uint32_t>(str[offset + 1] - '0');
}

[[nodiscard]] inline constexpr std::uint32_t get_seed_constexpr() {
    const char *t = __TIME__;
    return time_from_string(t, 0) * 3600 + time_from_string(t, 3) * 60 +
           time_from_string(t, 6);
}

struct LCEngine {
    std::uint32_t state;

    constexpr LCEngine(std::uint32_t seed) : state(seed) {}

    [[nodiscard]] constexpr std::uint32_t next() {
        state = (lce_a * state + lce_c) % lce_m;
        return state;
    }

    [[nodiscard]] constexpr double next_normalized() {
        return static_cast<double>(next()) / lce_m;
    }
};

template <typename T, std::size_t sz>
[[nodiscard]] inline constexpr auto uniform_distribution(T min, T max) {
    std::array<T, sz> dst{};
    LCEngine rng{get_seed_constexpr()};
    for (auto &el : dst)
        el = static_cast<T>(rng.next_normalized() * (max - min) + min);
    return dst;
}

template <mdspan_c in_t>
    requires(in_t::rank() == 0 && floating_point_c<typename in_t::value_type>)
inline void rand_impl(const in_t &in) noexcept {
    using dist_t = std::uniform_real_distribution<typename in_t::value_type>;

    static auto rd = std::random_device{};
    thread_local static auto gen = std::mt19937(rd());
    static auto dist = dist_t{0, 1};

    in() = dist(gen);
}

} // namespace detail

inline constexpr void
rand_to(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<decltype(In)>(In));
    using in_t = decltype(in);

    if constexpr (in_t::rank_dynamic() == 0) {
        if (std::is_constant_evaluated()) {
            using T = core::value_type_t<in_t>;

            if constexpr (in_t::rank() == 0) {
                constexpr auto data =
                    detail::uniform_distribution<T, 1>(0, 1)[0];
                in() = data;

            } else {
                constexpr auto data_size =
                    []<size_t... Is>(std::index_sequence<Is...>) {
                        return (in_t::static_extent(Is) * ...);
                    }(std::make_index_sequence<in_t::rank()>{});

                constexpr auto data =
                    ctmd::mdarray<T, typename in_t::extents_type>{
                        detail::uniform_distribution<T, data_size>(0, 1)};

                ctmd::copy_to(data, in);
            }

            return;
        }
    }

    core::batch(
        [](auto &&...elems) {
            detail::rand_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, mpmode, in);
}

template <floating_point_c T = float, extents_c exts_t = ctmd::extents<uint8_t>>
[[nodiscard]] inline constexpr auto
rand(const exts_t &exts = exts_t{},
     const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = empty<T>(exts);
    rand_to(out, mpmode);
    return out;
}

} // namespace random
} // namespace ctmd
