#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <md_c in_t, md_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void copy_impl(const in_t &in, out_t &out) noexcept {
    out[] = in[];
}

} // namespace detail

template <md_c in_t, md_c out_t>
    requires(!(in_t::rank() == 0 && out_t::rank() == 0))
inline constexpr void copy(const in_t &in, out_t &out,
                           const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<0>(in);
    const auto uout = core::submdspan_unit<0>(out);

    core::batch(
        detail::copy_impl<decltype(uin), decltype(uout)>, std::make_tuple(uin),
        std::make_tuple(uout), std::make_tuple(core::to_mdspan(in)),
        std::make_tuple(core::to_mdspan(out)), std::tuple<>{}, multi_process);
}

template <md_c in_t>
[[nodiscard]] inline constexpr auto
copy(const in_t &in, const bool multi_process = false) noexcept {
    auto out = mdarray<std::remove_const_t<typename in_t::element_type>,
                       typename in_t::extents_type>{in.extents()};
    ctmd::copy(in, out, multi_process);
    return out;
}

} // namespace ctmd
