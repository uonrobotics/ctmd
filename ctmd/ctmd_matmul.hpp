#pragma once

#include "linalg/ctmd_linalg_matmul.hpp"

namespace ctmd {

inline constexpr void matmul_to(auto &&...elems) noexcept {
    ctmd::linalg::matmul_to(std::forward<decltype(elems)>(elems)...);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto matmul(auto &&...elems) noexcept {
    return ctmd::linalg::matmul<dtype>(std::forward<decltype(elems)>(elems)...);
}

} // namespace ctmd
