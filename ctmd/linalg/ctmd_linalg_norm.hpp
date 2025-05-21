#pragma once

#include <cmath>

#include "../core/ctmd_core.hpp"
#include "../ctmd_multiply.hpp"
#include "../ctmd_sqrt.hpp"
#include "../ctmd_sum.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 0)
inline constexpr void norm_impl(const in_t &in, const out_t &out) noexcept {
    out() = 0;
    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        out() += in[i] * in[i];
    }
    out() = std::sqrt(out());
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void norm(InType &&In, OutType &&Out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    if (mpmode == MPMode::SIMD) [[unlikely]] {
        ctmd::sum<-1>(ctmd::multiply(in, in, mpmode), out, mpmode);
        ctmd::sqrt(out, out, mpmode);

    } else {
        core::batch(
            [](auto &&...elems) {
                detail::norm_impl(std::forward<decltype(elems)>(elems)...);
            },
            std::tuple{in, out},
            std::tuple{core::slice_from_last<1>(in.extents()),
                       extents<uint8_t>{}},
            std::tuple{}, mpmode);
    }
}

template <typename InType>
[[nodiscard]] inline constexpr auto
norm(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));

    if (mpmode == MPMode::SIMD) [[unlikely]] {
        return ctmd::sqrt(ctmd::sum<-1>(ctmd::multiply(in, in, mpmode), mpmode),
                          mpmode);

    } else {
        return core::batch(
            [](auto &&...elems) {
                detail::norm_impl(std::forward<decltype(elems)>(elems)...);
            },
            std::tuple{in},
            std::tuple{core::slice_from_last<1>(in.extents()),
                       extents<uint8_t>{}},
            std::tuple{}, mpmode);
    }
}

} // namespace linalg
} // namespace ctmd
