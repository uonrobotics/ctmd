#pragma once

#include <cmath>

#include "benchmark/benchmark.h"

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in = std::vector<T>(set_num, 1);
    auto out = std::vector<T>(set_num, 0);

    for (auto _ : state) {
        for (size_t i = 0; i < set_num; i++) {
            out[i] = std::sin(in[i]);
        }
    }

    state.SetComplexityN(state.range(0));
}
