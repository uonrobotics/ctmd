#pragma once

#include "empty.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto empty_like(auto &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    using dtype = typename decltype(in)::value_type;
    return ctmd::empty<dtype>(in.extents());
}

template <typename dtype>
[[nodiscard]] inline constexpr auto empty_like(auto &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    return ctmd::empty<dtype>(in.extents());
}

} // namespace ctmd
