/**
 * @file
 * @brief Element-wise addition utilities for CTMD.
 *
 * @details
 * Part of CTMD: Compile-Time Multi-Dimensional matrix library.
 * Licensed under the Apache License v2.0.
 *
 * @copyright
 * Copyright (2025) Uon Robotics, South Korea.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void add_impl(const in1_t &in1, const in2_t &in2,
                               const out_t &out) noexcept {
    out() = in1() + in2();
}

} // namespace detail

/**
 * @brief Add arguments element-wise.
 *
 * @param In1 md-like or scalar.
 * @param In2 md-like or scalar.
 * @param Out md-like or scalar.
 * @param mpmode (optional) Parallel execution mode. Default is MPMode::NONE.
 *
 * @note Equivalent to In1 + In2 = Out in terms of array broadcasting.
 *
 * @see ctmd::add(In1Type&&, In2Type&&, MPMode) for the in-place version that
 * returns the result.
 */
template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void add(In1Type &&In1, In2Type &&In2, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::add_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

/**
 * @brief Add arguments element-wise.
 *
 * @tparam dtype (optional) data type of the result. If void, deduced from
 * inputs.
 *
 * @param In1 md-like or scalar.
 * @param In2 md-like or scalar.
 * @param mpmode (optional) Parallel execution mode. Default is MPMode::NONE.
 *
 * @return mdarray or scalar.
 *
 * @note Equivalent to In1 + In2 = Out in terms of array broadcasting.
 *
 * @see ctmd::add(In1Type&&, In2Type&&, OutType&&, MPMode) for the in-place
 * version that modifies the output.
 */
template <typename dtype = void, typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
add(In1Type &&In1, In2Type &&In2, const MPMode mpmode = MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::add_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<In1Type>(In1)),
        core::to_const_mdspan(std::forward<In2Type>(In2)));
}

} // namespace ctmd
