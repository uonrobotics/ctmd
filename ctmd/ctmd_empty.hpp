#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename dtype, extents_c exts_t = extents<size_t>>
[[nodiscard]] inline constexpr auto
empty(const exts_t &exts = exts_t{}) noexcept {
    return core::create_out<dtype>(exts);
}

} // namespace ctmd
