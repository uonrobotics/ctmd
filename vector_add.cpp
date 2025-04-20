#include <iostream>

#define N 100

void vector_add(float *a, float *b, float *c, int n) {
#pragma acc parallel loop
    for (int i = 0; i < n; ++i)
        c[i] = a[i] + b[i];
}

int main() {
    float a[N], b[N], c[N];

    for (int i = 0; i < N; ++i) {
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

#pragma acc data copyin(a[0 : N], b[0 : N]) copyout(c[0 : N])
    {
        vector_add(a, b, c, N);
    }

    for (int i = 0; i < 10; ++i)
        std::cout << c[i] << std::endl;

    return 0;
}
