#pragma once

#include "benchmark/benchmark.h"

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 5e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    auto a = std::vector<T>(set_num, 1);
    auto b = std::vector<T>(set_num, 2);
    auto c = std::vector<T>(set_num, 0);

    T *pa = a.data();
    T *pb = b.data();
    T *pc = c.data();

#pragma omp target enter data map(to : pa[0 : set_num], pb[0 : set_num])       \
    map(alloc : pc[0 : set_num])

    for (auto _ : state) {
#pragma omp target teams distribute parallel for nowait map(                   \
        to : pa[0 : set_num], pb[0 : set_num]) map(from : pc[0 : set_num])
        for (size_t i = 0; i < set_num; ++i) {
            pc[i] = pa[i] + pb[i];
        }
    }

#pragma omp target exit data map(delete : pa[0 : set_num], pb[0 : set_num],    \
                                     pc[0 : set_num])

    state.SetComplexityN(state.range(0));
}
