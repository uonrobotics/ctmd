#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <md_c in1_t, md_c in2_t, md_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void add_impl(const in1_t &in1, const in2_t &in2,
                               out_t &out) noexcept {
    out[] = in1[] + in2[];
}

} // namespace detail

template <md_c in1_t, md_c in2_t, md_c out_t>
inline constexpr void add(const in1_t &in1, const in2_t &in2, out_t &out,
                          const bool multi_process = false) noexcept {
    const auto uin1 = core::submdspan_unit<0>(in1);
    const auto uin2 = core::submdspan_unit<0>(in2);
    const auto uout = core::submdspan_unit<0>(out);

    core::batch(
        detail::add_impl<decltype(uin1), decltype(uin2), decltype(uout)>,
        std::make_tuple(uin1, uin2), std::make_tuple(uout),
        std::make_tuple(core::to_mdspan(in1), core::to_mdspan(in2)),
        std::make_tuple(core::to_mdspan(out)), std::tuple<>{}, multi_process);
}

template <md_c in1_t, md_c in2_t>
[[nodiscard]] inline constexpr auto
add(const in1_t &in1, const in2_t &in2,
    const bool multi_process = false) noexcept {
    const auto uin1 = core::submdspan_unit<0>(in1);
    const auto uin2 = core::submdspan_unit<0>(in2);

    const auto uext =
        extents<std::common_type_t<typename decltype(uin1)::index_type,
                                   typename decltype(uin2)::index_type>>{};
    using uext_t = decltype(uext);

    auto uout_data = [&uext]<size_t... Is>(std::index_sequence<Is...>) {
        return mdarray<
            std::common_type_t<typename decltype(uin1)::element_type,
                               typename decltype(uin2)::element_type>,
            extents<typename uext_t::index_type, uext_t::static_extent(Is)...>>{
            uext.extent(Is)...};
    }(std::make_index_sequence<uext_t::rank()>());
    const auto uout = core::to_mdspan(uout_data);

    const auto results = core::batch_out(
        detail::add_impl<decltype(uin1), decltype(uin2), decltype(uout)>,
        std::make_tuple(uin1, uin2), std::make_tuple(uout),
        std::make_tuple(core::to_mdspan(in1), core::to_mdspan(in2)),
        std::tuple<>{}, multi_process);

    return std::get<0>(results);
}

} // namespace ctmd
