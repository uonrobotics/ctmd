#pragma once

#include "empty.hpp"
#include "fill.hpp"

namespace ctmd {

template <typename T, extents_c extents_t>
[[nodiscard]] inline constexpr auto
full(const T &val, const extents_t &extents = extents_t{},
     const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::empty<T>(extents);
    ctmd::fill(out, val, mpmode);
    return out;
}

} // namespace ctmd
