#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename in_t>
[[nodiscard]] inline constexpr bool all(const in_t &in) noexcept {
    if constexpr (!md_c<in_t>) {
        return static_cast<bool>(in);

    } else {
        if constexpr (in_t::rank() == 0) {
            return static_cast<bool>(in[]);

        } else {
            for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
                if (!all(core::submdspan_from_start(in, i))) {
                    return false;
                }
            }

            return true;
        }
    }
}

} // namespace ctmd
