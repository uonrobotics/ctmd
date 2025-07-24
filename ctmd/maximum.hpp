#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void maximum_impl(const in1_t &in1, const in2_t &in2,
                                   const out_t &out) noexcept {
    out() = std::max(static_cast<typename out_t::value_type>(in1()),
                     static_cast<typename out_t::value_type>(in2()));
}

} // namespace detail

inline constexpr void maximum_to(auto &&In1, auto &&In2, auto &&Out,
                                 const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::maximum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In1)>(In1)),
        core::to_const_mdspan(std::forward<decltype(In2)>(In2)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
maximum(auto &&In1, auto &&In2, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::maximum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In1)>(In1)),
        core::to_const_mdspan(std::forward<decltype(In2)>(In2)));
}

} // namespace ctmd
