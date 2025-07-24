#pragma once

#include "core/ctmd_core.hpp"

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

inline constexpr void
absolute_to(auto &&In, auto &&Out,
            const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::absolute_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
absolute(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::absolute_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)));
}

} // namespace ctmd
