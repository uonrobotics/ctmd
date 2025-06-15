#pragma once

#include "add.hpp"
#include "core/core.hpp"
#include "fill.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == out_t::rank() + 1)
inline constexpr void sum_impl(const in_t &in, const out_t &out) noexcept {
    ctmd::fill(out, 0);

    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        ctmd::add(out, core::submdspan_from_left(in, i), out);
    }
}

} // namespace detail

template <int64_t Axis, typename InType, typename OutType>
inline constexpr void sum(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    core::batch(
        [](auto &&...elems) {
            detail::sum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in, out},
        std::tuple{core::slice_from_right<rin_rank>(in.extents()),
                   core::slice_from_right<rin_rank - 1>(out.extents())},
        mpmode);
}

template <int64_t Axis, typename InType>
[[nodiscard]] inline constexpr auto
sum(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    return core::batch_out(
        [](auto &&...elems) {
            detail::sum_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in},
        std::tuple{core::slice_from_right<rin_rank>(in.extents()),
                   core::slice_from_right<rin_rank - 1>(in.extents())},
        mpmode);
}

} // namespace ctmd
