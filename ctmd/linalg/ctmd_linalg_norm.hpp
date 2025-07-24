#pragma once

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

#ifdef REAL_GCC
    out() = std::sqrt(out());

#else
    out() = ctmd::sqrt(out());

#endif
}

} // namespace detail

inline constexpr void
norm_to(auto &&In, auto &&Out,
        const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    const auto out = core::to_mdspan(std::forward<decltype(Out)>(Out));

    if (mpmode == ctmd::MPMode::SIMD) [[unlikely]] {
        ctmd::sum_to<-1>(ctmd::multiply(in, in, mpmode), out, mpmode);
        ctmd::sqrt_to(out, out, mpmode);
        return;
    }

    core::batch(
        [](auto &&...elems) {
            detail::norm_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<1, 0>{}, mpmode, in, out);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
norm(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    auto out = core::create_out<dtype>(std::index_sequence<1>{},
                                       ctmd::extents<uint8_t>{}, in);

    norm_to(in, out, mpmode);

    return out;
}

} // namespace linalg
} // namespace ctmd
