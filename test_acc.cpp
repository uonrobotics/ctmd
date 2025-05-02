#include <iostream>
#include <openacc.h>

int main() {
    int on_device = 0;

#pragma acc parallel copy(on_device)
    {
        on_device = acc_on_device(acc_device_nvidia);
    }

    if (on_device)
        std::cout << "✅ Running on GPU" << std::endl;
    else
        std::cout << "⚠️ Running on CPU (not offloaded)" << std::endl;

    return 0;
}
