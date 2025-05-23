#pragma once

#include "ctmd_empty.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto empty_like(InType &&In) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));
    using T = std::remove_cvref_t<typename decltype(in)::element_type>;
    return ctmd::empty<T>(in.extents());
}

} // namespace ctmd
