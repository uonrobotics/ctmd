#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c min_t, mdspan_c max_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && min_t::rank() == 0 && max_t::rank() == 0 &&
             out_t::rank() == 0)
inline constexpr void clip_impl(const in_t &in, const min_t &min,
                                const max_t &max, const out_t &out) noexcept {
    // NOTE: std::clamp is not used to match the behavior with original np.clip
    out() = in();

    if constexpr (!std::is_same_v<typename min_t::value_type, std::nullopt_t>) {
        out() = std::max(out(), static_cast<typename out_t::value_type>(min()));
    }

    if constexpr (!std::is_same_v<typename max_t::value_type, std::nullopt_t>) {
        out() = std::min(out(), static_cast<typename out_t::value_type>(max()));
    }
}

} // namespace detail

inline constexpr void
clip_to(auto &&In, auto &&Min, auto &&Max, auto &&Out,
        const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::clip_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0, 0>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)),
        core::to_const_mdspan(std::forward<decltype(Min)>(Min)),
        core::to_const_mdspan(std::forward<decltype(Max)>(Max)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
clip(auto &&In, auto &&Min, auto &&Max,
     const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::clip_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<0, 0, 0>{}, ctmd::extents<uint8_t>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In)>(In)),
        core::to_const_mdspan(std::forward<decltype(Min)>(Min)),
        core::to_const_mdspan(std::forward<decltype(Max)>(Max)));
}

} // namespace ctmd
