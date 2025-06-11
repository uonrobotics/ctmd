#pragma once

#include "core/core.hpp"

namespace ctmd {

template <md_c in_t>
[[nodiscard]] inline std::string to_string(const in_t &in) noexcept {
    const auto rin = core::to_const_mdspan(in);

    std::string str = "[";

    if constexpr (in_t::rank() == 0) {
        // do nothing

    } else if constexpr (in_t::rank() == 1) {
        for (typename in_t::size_type i = 0; i < rin.extent(0); i++) {
            str += std::to_string(rin[i]);
            if (i < rin.extent(0) - 1) {
                str += ", ";
            }
        }

    } else {
        for (typename in_t::size_type i = 0; i < rin.extent(0); i++) {
            const auto in_slice = core::submdspan_from_start(rin, i);
            str += to_string(in_slice);
            if (i < rin.extent(0) - 1) {
                str += ", ";
            }
        }
    }

    return str + "]";
}

template <extents_c in_t>
[[nodiscard]] inline std::string to_string(const in_t &in) noexcept {
    std::string str = "(";

    for (size_t i = 0; i < in_t::rank(); i++) {
        str += std::to_string(in.extent(i));
        if (i < in_t::rank() - 1) {
            str += ", ";
        }
    }

    return str + ")";
}

template <typename in_t>
[[nodiscard]] inline std::string to_string(const in_t &in) noexcept {
    return std::to_string(in);
}

} // namespace ctmd
