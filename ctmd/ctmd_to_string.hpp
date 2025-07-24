#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t>
[[nodiscard]] inline std::string to_string_impl(const in_t &in) noexcept {
    std::string str = "[";

    if constexpr (in_t::rank() == 0) {
        return std::to_string(in());

    } else if constexpr (in_t::rank() == 1) {
        for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
            str += std::to_string(in[i]);
            if (i < in.extent(0) - 1) {
                str += ", ";
            }
        }

    } else {
        for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
            const auto in_slice = core::submdspan_from_left(in, i);
            str += to_string_impl(in_slice);
            if (i < in.extent(0) - 1) {
                str += ", ";
            }
        }
    }

    return str + "]";
}

} // namespace detail

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

[[nodiscard]] inline std::string to_string(auto &&In) noexcept {
    return detail::to_string_impl(
        core::to_const_mdspan(std::forward<decltype(In)>(In)));
}

} // namespace ctmd
