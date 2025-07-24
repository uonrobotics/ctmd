#pragma once

#include "linalg/ctmd_linalg_matmul.hpp"

namespace ctmd {

template <typename... Elems>
    requires(std::is_void_v<
             decltype(ctmd::linalg::matmul(std::declval<Elems>()...))>)
inline constexpr void matmul_to(Elems &&...elems) noexcept {
    ctmd::linalg::matmul_to(std::forward<Elems>(elems)...);
}

template <typename dtype = void, typename... Elems>
    requires(!std::is_void_v<
             decltype(ctmd::linalg::matmul(std::declval<Elems>()...))>)
[[nodiscard]] inline constexpr auto matmul(Elems &&...elems) noexcept {
    return ctmd::linalg::matmul<dtype>(std::forward<Elems>(elems)...);
}

} // namespace ctmd
