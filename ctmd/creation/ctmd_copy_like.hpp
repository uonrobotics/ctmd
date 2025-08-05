#pragma once

#include "ctmd_copy.hpp"
#include "ctmd_empty_like.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto
copy_like(auto &&In1, auto &&In2,
          const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty_like(std::forward<decltype(In1)>(In1));
    ctmd::copy_to(std::forward<decltype(In2)>(In2), out, mpmode);
    return out;
}

template <typename dtype>
[[nodiscard]] inline constexpr auto
copy_like(auto &&In1, auto &&In2,
          const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty_like<dtype>(std::forward<decltype(In1)>(In1));
    ctmd::copy_to(std::forward<decltype(In2)>(In2), out, mpmode);
    return out;
}

} // namespace ctmd
