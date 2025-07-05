#pragma once

#include "empty.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto empty_like(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    using T = typename decltype(in)::value_type;
    return ctmd::empty<T>(in.extents());
}

template <typename T, typename InType>
[[nodiscard]] inline constexpr auto empty_like(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    return ctmd::empty<T>(in.extents());
}

} // namespace ctmd
