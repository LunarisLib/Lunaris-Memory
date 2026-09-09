#include <iostream>

#include <Lunaris/memory.h>

using namespace Lunaris::Memory;

int main() {
    Memory<int> mem(new int{15});
    Memory<int> mem_val = mem;

    *mem = 20;

    if (*mem != *mem_val) {
        std::printf("Values don't match\n");
        return 1;
    }
    if (mem.use_count() != 2 || mem_val.use_count() != 2) {
        std::printf("Use count is broken\n");
        return 1;
    }

    std::printf("PASSED\n");

    return 0;
}