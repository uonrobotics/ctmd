#pragma once

#include "../core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0 &&
             floating_point_c<typename out_t::value_type>)
inline constexpr void cos_impl(const in_t &in, const out_t &out) noexcept {
    out() = std::cos(in());
}

} // namespace detail

inline constexpr void
cos_to(auto &&In, auto &&Out,
       const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::cos_impl(std::forward<decltype(elems)>(elems)...);
        },
        mpmode, core::to_const_mdspan(std::forward<decltype(In)>(In)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
cos(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::cos_impl(std::forward<decltype(elems)>(elems)...);
        },
        ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)));
}

} // namespace ctmd
