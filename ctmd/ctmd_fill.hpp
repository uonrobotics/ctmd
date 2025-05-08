#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t>
    requires(in_t::rank() == 0)
inline constexpr void
fill_impl(const in_t &in, const typename in_t::element_type &val) noexcept {
    in[] = val;
}

} // namespace detail

template <md_c in_t>
inline constexpr void fill(in_t &in, const typename in_t::element_type &val,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin_exts = extents<typename in_t::index_type>{};

    auto bin = core::to_mdspan(in);
    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<in_t::rank(), MPMode::NONE>(
            detail::fill_impl<decltype(ubin)>, std::tuple{bin},
            std::tuple{val});
        break;

    case MPMode::CPUMP:
        core::batch<in_t::rank(), MPMode::CPUMP>(
            detail::fill_impl<decltype(ubin)>, std::tuple{bin},
            std::tuple{val});
        break;

    default:
        assert(false);
        break;
    }
}

} // namespace ctmd
