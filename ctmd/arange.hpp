#pragma once

#include "core/core.hpp"

namespace ctmd {

template <typename T = void, arithmetic_c start_t, arithmetic_c stop_t,
          arithmetic_c step_t = double>
[[nodiscard]] inline constexpr auto
arange(const start_t &start, const stop_t &stop,
       const step_t &step = (step_t)1) noexcept {
    using value_t = std::conditional_t<std::is_void_v<T>,
                                       std::common_type_t<start_t, stop_t>, T>;

    const size_t num = std::ceil((stop - start) / step);
    const value_t step_actual =
        static_cast<value_t>(start + step) - static_cast<value_t>(start);

    auto out = mdarray<value_t, dims<1>>{dims<1>{num}};

    out(0) = start;
    for (size_t i = 1; i < num; i++) {
        out(i) = out(i - 1) + step_actual;
    }

    return out;
}

template <typename T = void, arithmetic_c stop_t>
[[nodiscard]] inline constexpr auto arange(const stop_t &stop) noexcept {
    return arange<T>(0, stop);
}

} // namespace ctmd
