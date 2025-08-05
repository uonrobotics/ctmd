#pragma once

#include "../core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 0)
inline constexpr void max_impl(const in_t &in, const out_t &out) noexcept {
    out() = in(0);

    for (typename in_t::index_type i = 1; i < in.extent(0); i++) {
        if (in(i) > out()) {
            out() = in(i);
        }
    }
}

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == out_t::rank() + 1 && out_t::rank() > 0)
inline constexpr void max_impl(const in_t &in, const out_t &out) noexcept {
    for (typename in_t::index_type i = 0; i < in.extent(in_t::rank() - 1);
         i++) {
        max_impl(core::submdspan_from_right(in, i),
                 core::submdspan_from_right(out, i));
    }
}

template <mdspan_c in_t, arithmetic_c out_t>
inline constexpr void max_impl(const in_t &in, out_t &out) noexcept {
    // WARNING: DO NOT USE THIS FUNCTION DIRECTLY. OUT SHOULD BE INITIALIZED
    if constexpr (in_t::rank() == 0) {
        if (in() > out) {
            out = in();
        }

    } else {
        for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
            max_impl(core::submdspan_from_left(in, i), out);
        }
    }
}

} // namespace detail

template <int64_t Axis>
inline constexpr void
max(auto &&In, auto &&Out,
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
            detail::max_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank, rin_rank - 1>{}, mpmode, in, out);
}

template <int64_t Axis, typename dtype = void>
[[nodiscard]] inline constexpr auto
max(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));

    constexpr size_t in_rank = decltype(in)::rank();
    constexpr size_t rin_rank =
        in_rank -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(in_rank)) + (in_rank)) % in_rank);

    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::max_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<rin_rank>{},
        core::slice_from_right<rin_rank - 1>(in.extents()), mpmode, in);
}

[[nodiscard]] inline constexpr auto max(auto &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    using in_t = decltype(in);

    typename in_t::value_type out =
        std::numeric_limits<typename in_t::value_type>::lowest();

    detail::max_impl(in, out);

    return out;
}

} // namespace ctmd
