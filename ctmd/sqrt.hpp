#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

#ifndef REAL_GCC

template <floating_point_c dtype>
[[nodiscard]] inline constexpr dtype
sqrt_newton_raphson(const dtype &x, const dtype &curr,
                    const dtype &prev) noexcept {
    return (curr == prev)
               ? curr
               : sqrt_newton_raphson(x, (curr + x / curr) / (dtype)2, curr);
}

#endif

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void sqrt_impl(const in_t &in, const out_t &out) noexcept {
#ifdef REAL_GCC
    out() = std::sqrt(in());

#else
    // NOTE: std::abs is not constexpr in clang 16.

    if constexpr (floating_point_c<typename in_t::value_type>) {
        using dtype = typename in_t::value_type;

        out() = (in() >= 0 && in() < std::numeric_limits<dtype>::infinity())
                    ? sqrt_newton_raphson(in(), in(), (dtype)0)
                    : std::numeric_limits<dtype>::quiet_NaN();

    } else {
        using dtype = float;

        out() =
            (in() >= 0 && in() < std::numeric_limits<dtype>::infinity())
                ? sqrt_newton_raphson(static_cast<const dtype>(in()),
                                      static_cast<const dtype>(in()), (dtype)0)
                : std::numeric_limits<dtype>::quiet_NaN();
    }

#endif
}

} // namespace detail

inline constexpr void sqrt_to(auto &&In, auto &&Out,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::sqrt_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
sqrt(auto &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::sqrt_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)));
}

} // namespace ctmd
