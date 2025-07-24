#pragma once

#include "absolute.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void isclose_impl(const in1_t &in1, const in2_t &in2,
                                   const out_t &out, const double &rtol,
                                   const double &atol) noexcept {
    using dtype = typename in2_t::value_type;
    out() = ctmd::absolute(in1() - in2()) <=
            (static_cast<const dtype>(atol) +
             static_cast<const dtype>(rtol) * ctmd::absolute(in2()));
}

} // namespace detail

inline constexpr void isclose_to(auto &&In1, auto &&In2, auto &&Out,
                                 const double &rtol = 1e-05,
                                 const double &atol = 1e-08,
                                 const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [&](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)..., rtol,
                                 atol);
        },
        std::index_sequence<0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In1)>(In1)),
        core::to_const_mdspan(std::forward<decltype(In2)>(In2)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = int8_t>
[[nodiscard]] inline constexpr auto
isclose(auto &&In1, auto &&In2, const double &rtol = 1e-05,
        const double &atol = 1e-08,
        const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [&](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)..., rtol,
                                 atol);
        },
        std::index_sequence<0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In1)>(In1)),
        core::to_const_mdspan(std::forward<decltype(In2)>(In2)));
}

} // namespace ctmd
