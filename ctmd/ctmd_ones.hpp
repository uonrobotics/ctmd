#pragma once

#include "ctmd_full.hpp"

namespace ctmd {

template <typename dtype, extents_c exts_t = extents<size_t>>
[[nodiscard]] inline constexpr auto
zeros(const exts_t &exts = exts_t{},
      const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::full<dtype>(0, exts, mpmode);
}

} // namespace ctmd
