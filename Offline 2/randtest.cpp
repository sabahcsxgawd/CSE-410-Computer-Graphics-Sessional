#include <iostream>


static unsigned long int g_seed = 1;

inline int myRandom()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::cout << myRandom() % 256 << std::endl;
    }
    return 0;
}