#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 0)
inline constexpr void any_impl(const in_t &in, const out_t &out) noexcept {
    for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
        if (static_cast<bool>(in(i))) {
            out() = true;
            return;
        }
    }

    out() = false;
}

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == out_t::rank() + 1 && out_t::rank() > 0)
inline constexpr void any_impl(const in_t &in, const out_t &out) noexcept {
    for (typename in_t::index_type i = 0; i < in.extent(in_t::rank() - 1);
         i++) {
        any_impl(core::submdspan_from_right(in, i),
                 core::submdspan_from_right(out, i));
    }
}

} // namespace detail

template <int64_t Axis>
inline constexpr void
any(auto &&In, auto &&Out,
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
            detail::any_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank, rin_rank - 1>{}, mpmode, in, out);
}

template <int64_t Axis, typename dtype = int8_t>
[[nodiscard]] inline constexpr auto
any(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::any_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank>{},
        core::slice_from_right<rin_rank - 1>(in.extents()), mpmode, in);
}

[[nodiscard]] inline constexpr bool any(auto &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    using in_t = decltype(in);

    if constexpr (in_t::rank() == 0) {
        return static_cast<bool>(in());

    } else {
        for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
            if (any(core::submdspan_from_left(in, i))) {
                return true;
            }
        }

        return false;
    }
}

} // namespace ctmd
