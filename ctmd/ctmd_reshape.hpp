#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <extents_c extents_t>
[[nodiscard]] inline constexpr auto
reshape(auto &&In, const extents_t &new_extents = extents_t{}) noexcept {
    return core::reshape(core::to_mdspan(std::forward<decltype(In)>(In)),
                         new_extents);
}

} // namespace ctmd
