#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void absolute_impl(const in_t &in, const out_t &out) noexcept {
#ifdef REAL_GCC
    out() = std::abs(in());

#else
    // NOTE: std::abs is not constexpr in clang 16.
    out() = in() >= 0 ? in() : -in();

#endif
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void absolute(InType &&In, OutType &&Out,
                               const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::absolute_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)),
        core::to_const_mdspan(std::forward<OutType>(Out)));
}

template <typename InType>
[[nodiscard]] inline constexpr auto
absolute(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out(
        [](auto &&...elems) {
            detail::absolute_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)));
}

} // namespace ctmd
