#pragma once

#include <cmath>

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0 &&
             floating_point_c<typename out_t::element_type>)
inline constexpr void sin_impl(const in_t &in, const out_t &out) noexcept {
    out() = std::sin(in());
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void sin(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::sin_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in, out}, std::tuple{extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

template <typename InType>
[[nodiscard]] inline constexpr auto
sin(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));

    return core::batch_out<float>(
        [](auto &&...elems) {
            detail::sin_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in}, std::tuple{extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

} // namespace ctmd
