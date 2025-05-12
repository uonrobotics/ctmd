#pragma once

#include "benchmark/benchmark.h"

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 5e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in1 = std::vector<T>(set_num, 1);
    const auto in2 = std::vector<T>(set_num, 2);
    auto out = std::vector<T>(set_num, 0);

    for (auto _ : state) {
#pragma omp parallel for
        for (size_t i = 0; i < set_num; i++) {
            out[i] = in1[i] + in2[i];
        }
    }

    state.SetComplexityN(state.range(0));
}
