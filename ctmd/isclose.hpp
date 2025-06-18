#pragma once

#include "abs.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void isclose_impl(const in1_t &in1, const in2_t &in2,
                                   const out_t &out, const double &rtol,
                                   const double &atol) noexcept {
    using T = typename in2_t::value_type;
    out() = ctmd::abs(in1() - in2()) <=
            (static_cast<const T>(atol) +
             static_cast<const T>(rtol) * ctmd::abs(in2()));
}

} // namespace detail

template <typename In1Type, typename In2Type, typename OutType>
    requires(!arithmetic_c<OutType>)
inline constexpr void isclose(In1Type &&In1, In2Type &&In2, OutType &&Out,
                              const double &rtol = 1e-05,
                              const double &atol = 1e-08,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [&](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)..., rtol,
                                 atol);
        },
        std::index_sequence<0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
isclose(In1Type &&In1, In2Type &&In2, const double &rtol = 1e-05,
        const double &atol = 1e-08,
        const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out(
        [&](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)..., rtol,
                                 atol);
        },
        std::index_sequence<0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)));
}

} // namespace ctmd
