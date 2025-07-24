#pragma once

#include "linalg/ctmd_linalg_matvec.hpp"

namespace ctmd {

inline constexpr void matvec_to(auto &&...elems) noexcept {
    ctmd::linalg::matvec_to(std::forward<decltype(elems)>(elems)...);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto matvec(auto &&...elems) noexcept {
    return ctmd::linalg::matvec<dtype>(std::forward<decltype(elems)>(elems)...);
}

} // namespace ctmd
