#pragma once

#include <cmath>

#include "core/core.hpp"

namespace ctmd {
namespace detail {

#ifndef REAL_GCC

template <floating_point_c T>
[[nodiscard]] inline constexpr T sqrt_newton_raphson(const T &x, const T &curr,
                                                     const T &prev) noexcept {
    return (curr == prev)
               ? curr
               : sqrt_newton_raphson(x, (curr + x / curr) / (T)2, curr);
}

#endif

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void sqrt_impl(const in_t &in, const out_t &out) noexcept {
#ifdef REAL_GCC
    out() = std::sqrt(in());

#else
    // NOTE: std::abs is not constexpr in clang 16.

    if constexpr (floating_point_c<typename in_t::element_type>) {
        using T = typename in_t::element_type;

        out() = (in() >= 0 && in() < std::numeric_limits<T>::infinity())
                    ? sqrt_newton_raphson(in(), in(), (T)0)
                    : std::numeric_limits<T>::quiet_NaN();

    } else {
        using T = float;

        out() = (in() >= 0 && in() < std::numeric_limits<T>::infinity())
                    ? sqrt_newton_raphson(static_cast<T>(in()),
                                          static_cast<T>(in()), (T)0)
                    : std::numeric_limits<T>::quiet_NaN();
    }

#endif
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void sqrt(InType &&In, OutType &&Out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::sqrt_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename InType>
[[nodiscard]] inline constexpr auto
sqrt(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out(
        [](auto &&...elems) {
            detail::sqrt_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)));
}

} // namespace ctmd
