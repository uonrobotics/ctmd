#include <iostream>
#include <omp.h>

int main() {
#pragma omp target
    {
        printf("Hello from device\n");
    }

    std::cout << "Hello from host" << std::endl;
    return 0;
}