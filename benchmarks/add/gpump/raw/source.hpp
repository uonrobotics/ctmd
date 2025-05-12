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

    T *p1 = in1.data();
    T *p2 = in2.data();
    T *po = out.data();

#pragma omp target enter data map(to : p1[0 : set_num], p2[0 : set_num])       \
    map(alloc : po[0 : set_num])

    for (auto _ : state) {
#pragma omp target teams distribute parallel for nowait map(                   \
        to : p1[0 : set_num], p2[0 : set_num]) map(from : po[0 : set_num])
        for (size_t i = 0; i < set_num; i++) {
            po[i] = p1[i] + p2[i];
        }
    }

#pragma omp target exit data map(delete : p1[0 : set_num], p2[0 : set_num],    \
                                     po[0 : set_num])

    state.SetComplexityN(state.range(0));
}
