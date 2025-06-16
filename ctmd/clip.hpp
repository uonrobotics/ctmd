#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c min_t, mdspan_c max_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && min_t::rank() == 0 && max_t::rank() == 0 &&
             out_t::rank() == 0)
inline constexpr void clip_impl(const in_t &in, const min_t &min,
                                const max_t &max, const out_t &out) noexcept {
    using element_t = std::remove_cvref_t<decltype(in())>;

    out() = std::clamp(in(), static_cast<element_t>(min()),
                       static_cast<element_t>(max()));
}

} // namespace detail

template <typename InType, typename MinType, typename MaxType, typename OutType>
inline constexpr void clip(InType &&In, MinType &&Min, MaxType &&Max,
                           OutType &&Out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto min = core::to_const_mdspan(std::forward<MinType>(Min));
    const auto max = core::to_const_mdspan(std::forward<MaxType>(Max));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::clip_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0, 0>{}, mpmode, in, min, max, out);
}

template <typename InType, typename MinType, typename MaxType>
[[nodiscard]] inline constexpr auto
clip(InType &&In, MinType &&Min, MaxType &&Max,
     const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto min = core::to_const_mdspan(std::forward<MinType>(Min));
    const auto max = core::to_const_mdspan(std::forward<MaxType>(Max));

    return core::batch_out(
        [](auto &&...elems) {
            detail::clip_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0>{}, ctmd::extents<uint8_t>{}, mpmode, in,
        min, max);
}

} // namespace ctmd
