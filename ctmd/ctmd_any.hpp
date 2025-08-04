#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool any(auto &&In) noexcept {
    // TODO: Implement axis
    const auto in = core::to_const_mdspan(std::forward<decltype(In)>(In));
    using in_t = decltype(in);

    if constexpr (in_t::rank() == 0) {
        return static_cast<bool>(in());

    } else {
        for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
            if (ctmd::any(core::submdspan_from_left(in, i))) {
                return true;
            }
        }

        return false;
    }
}

} // namespace ctmd
