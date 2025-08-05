#pragma once

#include "ctmd_empty.hpp"
#include "ctmd_fill.hpp"

namespace ctmd {

template <typename dtype, extents_c extents_t>
[[nodiscard]] inline constexpr auto
full(const dtype &val, const extents_t &extents = extents_t{},
     const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty<dtype>(extents);
    ctmd::fill(out, val, mpmode);
    return out;
}

} // namespace ctmd
