#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr bool all(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    using in_t = decltype(in);

    if constexpr (!mdspan_c<in_t>) {
        return static_cast<bool>(in);

    } else {
        if constexpr (in_t::rank() == 0) {
            return static_cast<bool>(in());

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
