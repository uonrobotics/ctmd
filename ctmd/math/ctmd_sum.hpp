#pragma once

#include "../core/ctmd_core.hpp"
#include "../creation/ctmd_fill.hpp"
#include "ctmd_add.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == out_t::rank() + 1)
inline constexpr void sum_impl(const in_t &in, const out_t &out) noexcept {
    ctmd::fill(out, 0);

    for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
        ctmd::add_to(out, core::submdspan_from_left(in, i), out);
    }
}

} // namespace detail

template <int64_t Axis>
inline constexpr void
sum_to(auto &&In, auto &&Out,
       const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    const auto out = core::to_mdspan(std::forward<decltype(Out)>(Out));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    core::batch(
        [](auto &&...elems) {
            detail::sum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank, rin_rank - 1>{}, mpmode, in, out);
}

template <int64_t Axis, typename dtype = void>
[[nodiscard]] inline constexpr auto
sum(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::sum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank>{},
        core::slice_from_right<rin_rank - 1>(in.extents()), mpmode, in);
}

} // namespace ctmd
