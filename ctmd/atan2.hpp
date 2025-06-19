#pragma once

#include <cmath>

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0 &&
             floating_point_c<typename out_t::value_type>)
inline constexpr void atan2_impl(const in1_t &in1, const in2_t &in2,
                                 const out_t &out) noexcept {
    using value_t = typename out_t::value_type;

    out() =
        std::atan2(static_cast<value_t>(in1()), static_cast<value_t>(in2()));
}

} // namespace detail

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void atan2(In1Type &&In1, In2Type &&In2, OutType &&Out,
                            const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::atan2_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
atan2(In1Type &&In1, In2Type &&In2,
      const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<float>(
        [](auto &&...elems) {
            detail::atan2_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)));
}

} // namespace ctmd
