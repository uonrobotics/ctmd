#pragma once

#include "../multiply.hpp"
#include "../sqrt.hpp"
#include "../sum.hpp"

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

#ifdef REAL_GCC
    out() = std::sqrt(out());

#else
    out() = ctmd::sqrt(out());

#endif
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void norm(InType &&In, OutType &&Out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    if (mpmode == MPMode::SIMD) [[unlikely]] {
        ctmd::sum<-1>(ctmd::multiply(in, in, mpmode), out, mpmode);
        ctmd::sqrt(out, out, mpmode);
        return;
    }

    core::batch(
        [](auto &&...elems) {
            detail::norm_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<1, 0>{}, mpmode, in, out);
}

template <typename InType>
[[nodiscard]] inline constexpr auto
norm(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));

    if (mpmode == MPMode::SIMD) [[unlikely]] {
        return ctmd::sqrt(ctmd::sum<-1>(ctmd::multiply(in, in, mpmode), mpmode),
                          mpmode);
    }

    return core::batch_out(
        [](auto &&...elems) {
            detail::norm_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<1>{}, ctmd::extents<uint8_t>{}, mpmode, in);
}

} // namespace linalg
} // namespace ctmd
