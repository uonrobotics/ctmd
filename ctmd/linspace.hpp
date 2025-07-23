#pragma once

#include "./core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c start_t, mdspan_c stop_t, mdspan_c out_t>
    requires(start_t::rank() == 0 && stop_t::rank() == 0 && out_t::rank() == 1)
inline constexpr void linspace_impl(const start_t &start, const stop_t &stop,
                                    const out_t &out,
                                    const bool endpoint) noexcept {
    using value_t = typename out_t::value_type;
    using index_t = typename out_t::index_type;

    const index_t num = out.extent(0);

    if (num == 0) [[unlikely]] {
        // do nothing

    } else if (num == 1) [[unlikely]] {
        out(0) = static_cast<value_t>(start());

    } else {
        const value_t start_val = static_cast<value_t>(start());
        const value_t stop_val = static_cast<value_t>(stop());
        const value_t step =
            (stop_val - start_val) / (endpoint ? num - 1 : num);

        for (index_t i = 0; i < num; i++) {
            out(i) = start_val + step * i;
        }
    }
}

} // namespace detail

template <int64_t Axis = 0, typename start_in_t, typename stop_in_t,
          typename out_in_t>
    requires(!extents_c<out_in_t>)
inline constexpr void linspace(start_in_t &&start_in, stop_in_t &&stop_in,
                               out_in_t &&out_in,
                               const bool endpoint = true) noexcept {
    const auto start =
        core::to_const_mdspan(std::forward<start_in_t>(start_in));
    const auto stop = core::to_const_mdspan(std::forward<stop_in_t>(stop_in));
    const auto out = core::to_mdspan(std::forward<out_in_t>(out_in));
    using start_t = decltype(start);
    using stop_t = decltype(stop);
    using out_t = decltype(out);

    static_assert(start_t::rank() == stop_t::rank() &&
                      start_t::rank() + 1 == out_t::rank(),
                  "linspace does not support broadcasting");
    // TODO: support broadcasting without changing rank.

    if constexpr (start_t::rank() == 0) {
        detail::linspace_impl(start, stop, out, endpoint);

    } else {
        constexpr size_t out_rank = out_t::rank();
        constexpr size_t axis = static_cast<size_t>(
            ((Axis % static_cast<int64_t>(out_rank)) + (out_rank)) % out_rank);
        constexpr size_t lspace = axis == 0 ? 1 : 0;

        assert(start.extent(0) == stop.extent(0));
        assert(start.extent(0) == out.extent(lspace));

        for (typename out_t::index_type i = 0; i < out.extent(lspace); i++) {
            linspace<axis - (lspace == 0 ? 1 : 0)>(
                core::submdspan_from_left(start, i),
                core::submdspan_from_left(stop, i),
                core::submdspan_from_left<lspace>(out, i), endpoint);
        }
    }
}

template <int64_t Axis = 0, extents_c exts_t = extents<uint8_t, 50>,
          typename dtype = void, typename start_in_t, typename stop_in_t>
    requires(exts_t::rank() == 1)
[[nodiscard]] inline constexpr auto
linspace(start_in_t &&start_in, stop_in_t &&stop_in,
         const exts_t &exts = exts_t{}, const bool endpoint = true) noexcept {
    const auto start =
        core::to_const_mdspan(std::forward<start_in_t>(start_in));
    const auto stop = core::to_const_mdspan(std::forward<stop_in_t>(stop_in));
    using start_t = decltype(start);
    using stop_t = decltype(stop);

    using value_t =
        std::conditional_t<!std::is_void_v<dtype>, dtype,
                           core::common_type_t<typename start_t::value_type,
                                               typename stop_t::value_type>>;

    constexpr size_t out_rank = start_t::rank() + 1;
    constexpr size_t axis = static_cast<size_t>(
        ((Axis % static_cast<int64_t>(out_rank)) + (out_rank)) % out_rank);

    const auto bexts = start.extents();
    auto out = core::create_out<value_t>(core::concatenate(
        core::slice_from_left<axis>(bexts), exts,
        core::slice_from_right<decltype(bexts)::rank() - axis>(bexts)));

    linspace<Axis>(start, stop, out, endpoint);

    return out;
}

} // namespace ctmd
