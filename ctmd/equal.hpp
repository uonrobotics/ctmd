#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void equal_impl(const in1_t &in1, const in2_t &in2,
                                 const out_t &out) noexcept {
    out() = (in1() == in2());
}

} // namespace detail

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void equal(In1Type &&In1, In2Type &&In2, OutType &&Out,
                            const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<In2Type>(In2));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::equal_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2, out},
        std::tuple{extents<uint8_t>{}, extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
equal(In1Type &&In1, In2Type &&In2,
      const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<In2Type>(In2));

    return core::batch_out(
        [](auto &&...elems) {
            detail::equal_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2},
        std::tuple{extents<uint8_t>{}, extents<uint8_t>{}, extents<uint8_t>{}},
        std::tuple{}, mpmode);
}

} // namespace ctmd
