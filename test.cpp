#include <iostream>
#include <omp.h>
#include <openacc.h>

// int main() {
//     int on_device = acc_on_device(acc_device_nvidia);

//     if (on_device)
//         std::cout << "✅ Running on GPU" << std::endl;
//     else
//         std::cout << "⚠️ Running on CPU (not offloaded)" << std::endl;

//     return 0;
// }

int main() {
#pragma omp target
    {
        printf("Hello from device\n");
    }

    std::cout << "Hello from host" << std::endl;
    return 0;
}