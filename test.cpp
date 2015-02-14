#include <iostream>
#include <botan/auto_rng.h>
#include <botan/init.h>

int main()
{
    Botan::LibraryInitializer init;
    Botan::AutoSeeded_RNG rng;
    std::cout << "hello world" << rng.next_byte() << std::endl;
}
