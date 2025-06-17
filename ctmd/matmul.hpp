#pragma once

#include "linalg/matmul.hpp"

namespace ctmd {

template <typename... Elems>
    requires(std::is_void_v<
             decltype(ctmd::linalg::matmul(std::declval<Elems>()...))>)
inline constexpr void matmul(Elems &&...elems) noexcept {
    ctmd::linalg::matmul(std::forward<Elems>(elems)...);
}

template <typename... Elems>
    requires(!std::is_void_v<
             decltype(ctmd::linalg::matmul(std::declval<Elems>()...))>)
[[nodiscard]] inline constexpr auto matmul(Elems &&...elems) noexcept {
    return ctmd::linalg::matmul(std::forward<Elems>(elems)...);
}

} // namespace ctmd
