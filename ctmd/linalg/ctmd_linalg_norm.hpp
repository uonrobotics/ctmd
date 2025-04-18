#pragma once

#include "../core/ctmd_core.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <md_c in_t, md_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 1)
inline constexpr void norm_impl(const in_t &in, out_t &out) noexcept {
    out[0] = 0;
    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        out[0] += in[i] * in[i];
    }
    out[0] = std::sqrt(out[0]);
}

} // namespace detail

template <md_c in_t, md_c out_t>
    requires(in_t::rank() >= 1 && out_t::rank() >= 1)
inline constexpr void norm(const in_t &in, out_t &out,
                           const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<1>(in);
    const auto uout = core::submdspan_unit<1>(out);

    core::batch(
        detail::norm_impl<decltype(uin), decltype(uout)>, std::make_tuple(uin),
        std::make_tuple(uout), std::make_tuple(core::to_mdspan(in)),
        std::make_tuple(core::to_mdspan(out)), std::tuple<>{}, multi_process);
}

template <md_c in_t>
    requires(in_t::rank() >= 1)
[[nodiscard]] inline constexpr auto
norm(const in_t &in, const bool multi_process = false) noexcept {
    const auto uin = core::submdspan_unit<1>(in);
    const auto uout =
        mdarray<std::remove_const_t<typename decltype(uin)::element_type>,
                extents<typename decltype(uin)::index_type, 1>>{}
            .to_mdspan();

    const auto results = core::batch_out(
        detail::norm_impl<decltype(uin), decltype(uout)>, std::make_tuple(uin),
        std::make_tuple(uout), std::make_tuple(core::to_mdspan(in)),
        std::tuple<>{}, multi_process);

    return std::get<0>(results);
}

} // namespace linalg
} // namespace ctmd
