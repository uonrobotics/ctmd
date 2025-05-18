#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename in_t>
[[nodiscard]] inline constexpr bool all(in_t &&in) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));
    using rin_t = decltype(rin);

    if constexpr (!mdspan_c<rin_t>) {
        return static_cast<bool>(in);

    } else {
        if constexpr (rin_t::rank() == 0) {
            return static_cast<bool>(rin());

        } else {
            for (typename rin_t::size_type i = 0; i < rin.extent(0); i++) {
                if (!all(core::submdspan_from_start(rin, i))) {
                    return false;
                }
            }

            return true;
        }
    }
}

} // namespace ctmd
