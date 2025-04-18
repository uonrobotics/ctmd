#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <md_c in_t>
    requires(in_t::rank() == 0)
inline constexpr void
fill_impl(in_t &in, const typename in_t::element_type &val) noexcept {
    in[] = val;
}

} // namespace detail

template <md_c in_t>
inline constexpr void fill(in_t &in, const typename in_t::element_type &val,
                           const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<0>(in);

    core::batch(detail::fill_impl<decltype(uin)>, std::make_tuple(uin),
                std::tuple<>{}, std::make_tuple(core::to_mdspan(in)),
                std::tuple<>{}, std::make_tuple(val), multi_process);
}

} // namespace ctmd
