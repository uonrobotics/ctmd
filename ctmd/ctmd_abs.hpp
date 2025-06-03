#pragma once

#include <cmath>

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void abs_impl(const in_t &in, const out_t &out) noexcept {
#ifdef REAL_GCC
    out() = std::abs(in());

#else
    // NOTE: std::abs is not constexpr in clang 16.
    out() = in() >= 0 ? in() : -in();

#endif
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void abs(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto out = core::to_const_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::abs_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in, out}, std::tuple{extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

template <typename InType>
[[nodiscard]] inline constexpr auto
abs(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));

    return core::batch_out(
        [](auto &&...elems) {
            detail::abs_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in}, std::tuple{extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

} // namespace ctmd
