#pragma once

#include "benchmark/benchmark.h"

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e6;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    auto a = std::vector<T>(set_num, 1);
    auto b = std::vector<T>(set_num, 2);
    auto c = std::vector<T>(set_num, 0);

    for (auto _ : state) {
        for (size_t i = 0; i < set_num; ++i) {
            c[i] = a[i] + b[i];
        }
    }

    state.SetComplexityN(state.range(0));
}
