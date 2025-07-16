#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0 &&
             floating_point_c<typename out_t::value_type>)
inline constexpr void sin_impl(const in_t &in, const out_t &out) noexcept {
    out() = std::sin(in());
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void sin(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::sin_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename dtype = void, typename InType>
[[nodiscard]] inline constexpr auto
sin(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::sin_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)));
}

} // namespace ctmd
