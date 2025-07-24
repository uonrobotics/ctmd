#pragma once

#include "../ctmd_add.hpp"
#include "../ctmd_multiply.hpp"
#include "ctmd_random_rand.hpp"

namespace ctmd {
namespace random {

inline constexpr void
uniform_to(auto &&In, const double &low = 0, const double &high = 1,
           const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<decltype(In)>(In));

    using T = typename decltype(in)::value_type;

    random::rand_to(in, mpmode);

    if (mpmode == ctmd::MPMode::SIMD) [[unlikely]] {
        ctmd::multiply_to(in, static_cast<const T>(high - low), in);
        ctmd::add_to(in, static_cast<const T>(low), in);
        return;
    }

    core::batch(
        [&](auto &&in) {
            in() = static_cast<const T>(high - low) * in() +
                   static_cast<const T>(low);
        },
        std::index_sequence<0>{}, mpmode, in);
}

template <floating_point_c T = float, extents_c exts_t = ctmd::extents<uint8_t>>
[[nodiscard]] inline constexpr auto
uniform(const exts_t &exts = exts_t{}, const double &low = 0,
        const double &high = 1,
        const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = empty<T>(exts);
    uniform_to(out, low, high, mpmode);
    return out;
}

} // namespace random
} // namespace ctmd
