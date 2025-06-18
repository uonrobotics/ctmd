#pragma once

#include "../add.hpp"
#include "../multiply.hpp"
#include "rand.hpp"

namespace ctmd {
namespace random {

template <typename InType>
inline constexpr void uniform(InType &&In, const double &low = 0,
                              const double &high = 1,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));

    using T = typename decltype(in)::value_type;

    random::rand(in, mpmode);

    if (mpmode == MPMode::SIMD) [[unlikely]] {
        ctmd::multiply(in, static_cast<const T>(high - low), in);
        ctmd::add(in, static_cast<const T>(low), in);
        return;
    }

    core::batch(
        [&](auto &&in) {
            in() = static_cast<const T>(high - low) * in() +
                   static_cast<const T>(low);
        },
        std::index_sequence<0>{}, mpmode, in);
}

template <floating_point_c T = float, extents_c exts_t = extents<uint8_t, 1>>
[[nodiscard]] inline constexpr auto
uniform(const exts_t &exts = exts_t{}, const double &low = 0,
        const double &high = 1, const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = empty<T>(exts);
    uniform(out, low, high, mpmode);
    return out;
}

} // namespace random
} // namespace ctmd
