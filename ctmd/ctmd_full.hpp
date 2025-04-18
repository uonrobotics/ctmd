#pragma once

#include "core/ctmd_core.hpp"
#include "ctmd_fill.hpp"

namespace ctmd {

template <typename T, extents_c extents_t>
[[nodiscard]] inline constexpr auto
full(const T &val, const extents_t &extents = extents_t{},
     const bool multi_process = false) noexcept {
    auto out = mdarray<T, extents_t>{extents};
    fill(out, val, multi_process);
    return out;
}

} // namespace ctmd
