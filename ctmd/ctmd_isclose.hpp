#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void isclose_impl(const in1_t &in1, const in2_t &in2,
                                   const out_t &out, const double &rtol,
                                   const double &atol) noexcept {
    using T = std::remove_cvref_t<decltype(in2())>;
    out() = std::abs(in1() - in2()) <= ((T)atol + (T)rtol * std::abs(in2()));
}

} // namespace detail

template <typename In1Type, typename In2Type, typename OutType>
    requires(!arithmetic_c<OutType>)
inline constexpr void isclose(In1Type &&In1, In2Type &&In2, OutType &&Out,
                              const double &rtol = 1e-05,
                              const double &atol = 1e-08,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_mdspan(std::forward<In2Type>(In2));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2, out},
        std::tuple{extents<uint8_t>{}, extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{rtol, atol}, mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
isclose(In1Type &&In1, In2Type &&In2, const double &rtol = 1e-05,
        const double &atol = 1e-08,
        const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_mdspan(std::forward<In2Type>(In2));

    return core::batch(
        [](auto &&...elems) {
            detail::isclose_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2},
        std::tuple{extents<uint8_t>{}, extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{rtol, atol}, mpmode);
}

} // namespace ctmd
