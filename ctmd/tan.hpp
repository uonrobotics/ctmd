#pragma once

#include <cmath>

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0 &&
             floating_point_c<typename out_t::value_type>)
inline constexpr void tan_impl(const in_t &in, const out_t &out) noexcept {
    out() = std::tan(in());
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void tan(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::tan_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename InType>
[[nodiscard]] inline constexpr auto
tan(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<float>(
        [](auto &&...elems) {
            detail::tan_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)));
}

} // namespace ctmd
