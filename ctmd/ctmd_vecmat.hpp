#pragma once

#include "linalg/ctmd_linalg_vecmat.hpp"

namespace ctmd {

inline constexpr void vecmat_to(auto &&...elems) noexcept {
    ctmd::linalg::vecmat_to(std::forward<decltype(elems)>(elems)...);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto vecmat(auto &&...elems) noexcept {
    return ctmd::linalg::vecmat<dtype>(std::forward<decltype(elems)>(elems)...);
}

} // namespace ctmd
