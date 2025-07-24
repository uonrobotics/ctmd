#pragma once

#include "linalg/matvec.hpp"

namespace ctmd {

template <typename... Elems>
    requires(std::is_void_v<
             decltype(ctmd::linalg::matvec(std::declval<Elems>()...))>)
inline constexpr void matvec_to(Elems &&...elems) noexcept {
    ctmd::linalg::matvec_to(std::forward<Elems>(elems)...);
}

template <typename dtype = void, typename... Elems>
    requires(!std::is_void_v<
             decltype(ctmd::linalg::matvec(std::declval<Elems>()...))>)
[[nodiscard]] inline constexpr auto matvec(Elems &&...elems) noexcept {
    return ctmd::linalg::matvec<dtype>(std::forward<Elems>(elems)...);
}

} // namespace ctmd
