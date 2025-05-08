#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/ctmd.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto a = md::full<T, md::dims<1>>(1, md::dims<1>{set_num});
    const auto b = md::full<T, md::dims<1>>(2, md::dims<1>{set_num});
    auto c = md::mdarray<T, md::dims<1>>{md::dims<1>{set_num}};

    for (auto _ : state) {
        md::add(a, b, c, md::MPMode::GPUMP);
    }

    state.SetComplexityN(state.range(0));
}
