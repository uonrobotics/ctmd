#pragma once

#include "empty.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto empty_like(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    using dtype = typename decltype(in)::value_type;
    return ctmd::empty<dtype>(in.extents());
}

template <typename dtype, typename InType>
[[nodiscard]] inline constexpr auto empty_like(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    return ctmd::empty<dtype>(in.extents());
}

} // namespace ctmd
