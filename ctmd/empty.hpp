#pragma once

#include "core/core.hpp"

namespace ctmd {

template <typename T, extents_c exts_t = extents<size_t>>
[[nodiscard]] inline constexpr auto
empty(const exts_t &exts = exts_t{}) noexcept {
    return core::detail::create_out<T>(exts);
}

} // namespace ctmd
