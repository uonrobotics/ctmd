#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <md_c in_t, md_c min_t, md_c max_t, md_c out_t>
    requires(in_t::rank() == 0 && min_t::rank() == 0 && max_t::rank() == 0 &&
             out_t::rank() == 0)
inline constexpr void clip_impl(const in_t &in, const min_t &min,
                                const max_t &max, out_t &out) noexcept {
    out[] = std::clamp(in[], min[], max[]);
}

} // namespace detail

template <md_c in_t, md_c min_t, md_c max_t, md_c out_t>
inline constexpr void clip(const in_t &in, const min_t &min, const max_t &max,
                           out_t &out,
                           const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<0>(in);
    const auto umin = core::submdspan_unit<0>(min);
    const auto umax = core::submdspan_unit<0>(max);
    const auto uout = core::submdspan_unit<0>(out);

    core::batch(detail::clip_impl<decltype(uin), decltype(umin), decltype(umax),
                                  decltype(uout)>,
                std::make_tuple(uin, umin, umax), std::make_tuple(uout),
                std::make_tuple(core::to_mdspan(in), core::to_mdspan(min),
                                core::to_mdspan(max)),
                std::make_tuple(core::to_mdspan(out)), std::tuple<>{},
                multi_process);
}

template <md_c in_t, md_c min_t, md_c max_t>
[[nodiscard]] inline constexpr auto
clip(const in_t &in, const min_t &min, const max_t &max,
     const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<0>(in);
    const auto umin = core::submdspan_unit<0>(min);
    const auto umax = core::submdspan_unit<0>(max);

    const auto uext = extents<typename decltype(uin)::index_type>{};
    using uext_t = decltype(uext);

    auto uout_data = [&uext]<size_t... Is>(std::index_sequence<Is...>) {
        return mdarray<
            std::remove_const_t<typename decltype(uin)::element_type>,
            extents<typename uext_t::index_type, uext_t::static_extent(Is)...>>{
            uext.extent(Is)...};
    }(std::make_index_sequence<uext_t::rank()>());
    const auto uout = core::to_mdspan(uout_data);

    const auto results = core::batch_out(
        detail::clip_impl<decltype(uin), decltype(umin), decltype(umax),
                          decltype(uout)>,
        std::make_tuple(uin, umin, umax), std::make_tuple(uout),
        std::make_tuple(core::to_mdspan(in), core::to_mdspan(min),
                        core::to_mdspan(max)),
        std::tuple<>{}, multi_process);

    return std::get<0>(results);
}

} // namespace ctmd
