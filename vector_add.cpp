// #include <array>
// #include <iostream>

// int main() {
//     using T = float;
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

// #pragma acc data copyin(pa[0 : set_num], pb[0 : set_num])                      \
//     copyout(pc[0 : set_num])
// #pragma acc parallel loop
//     for (size_t i = 0; i < set_num; i++) {
//         pc[i] = pa[i] + pb[i];
//     }

//     for (size_t i = 0; i < set_num; i++) {
//         std::cout << c[i] << " ";
//     }
//     std::cout << std::endl;

//     return 0;
// }

#include <iostream>
#include <omp.h>

constexpr int N = 1024;

int main() {
    float a[N], b[N], c[N];

    for (int i = 0; i < N; ++i) {
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

#pragma omp target teams distribute parallel for map(to : a[0 : N], b[0 : N])  \
    map(from : c[0 : N])
    for (int i = 0; i < N; ++i) {
        c[i] = a[i] + b[i];
    }

    std::cout << c[0] << ", " << c[N - 1] << std::endl;
    return 0;
}
