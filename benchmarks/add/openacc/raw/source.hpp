// #pragma once

// #include <array>

// #include "benchmark/benchmark.h"

// constexpr size_t RANGE_START = 1;
// constexpr size_t RANGE_END = 1e6;
// constexpr size_t RANGE_MULTIPLIER = 10;

// #pragma acc routine seq
// inline float add(float a, float b) { return a + b; }

// template <typename T> inline void test(benchmark::State &state) noexcept {
//     constexpr size_t set_num = 10;

//     auto a = std::array<T, set_num>{};
//     auto b = std::array<T, set_num>{};
//     auto c = std::array<T, set_num>{};

//     for (size_t i = 0; i < set_num; ++i) {
//         a[i] = 1.0f;
//         b[i] = 2.0f;
//     }

//     T *pa = a.data();
//     T *pb = b.data();
//     T *pc = c.data();

//     for (auto _ : state) {
// #pragma acc data copyin(pa[0 : set_num], pb[0 : set_num]) \
//     copyout(pc[0 : set_num])
// #pragma acc parallel loop
//         for (size_t i = 0; i < set_num; i++) {
//             pc[i] = add(pa[i], pb[i]);
//         }
//     }

//     state.SetComplexityN(state.range(0));
// }

#pragma once

#include <array>
#include <omp.h>

#include "benchmark/benchmark.h"

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e6;
constexpr size_t RANGE_MULTIPLIER = 10;

#pragma omp declare target
inline float add(float a, float b) { return a + b; }
#pragma omp end declare target

template <typename T> inline void test(benchmark::State &state) noexcept {
    constexpr size_t set_num = 10;

    auto a = std::array<T, set_num>{};
    auto b = std::array<T, set_num>{};
    auto c = std::array<T, set_num>{};

    for (size_t i = 0; i < set_num; ++i) {
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

    T *pa = a.data();
    T *pb = b.data();
    T *pc = c.data();

    for (auto _ : state) {
#pragma omp target teams distribute parallel for map(                          \
        to : pa[0 : set_num], pb[0 : set_num]) map(from : pc[0 : set_num])
        for (size_t i = 0; i < set_num; ++i) {
            pc[i] = add(pa[i], pb[i]);
        }
    }

    state.SetComplexityN(state.range(0));
}
