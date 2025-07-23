#pragma once

#include "./linalg/vecmat.hpp"

namespace ctmd {

template <typename... Elems>
    requires(std::is_void_v<
             decltype(ctmd::linalg::vecmat(std::declval<Elems>()...))>)
inline constexpr void vecmat(Elems &&...elems) noexcept {
    ctmd::linalg::vecmat(std::forward<Elems>(elems)...);
}

template <typename dtype = void, typename... Elems>
    requires(!std::is_void_v<
             decltype(ctmd::linalg::vecmat(std::declval<Elems>()...))>)
[[nodiscard]] inline constexpr auto vecmat(Elems &&...elems) noexcept {
    return ctmd::linalg::vecmat<dtype>(std::forward<Elems>(elems)...);
}

} // namespace ctmd
