#include "systemc.h"

#include "common_rand.h"

// Generate a random delay (with power-law distribution) to aid testing and stress the protocol
int rand_ps()
{
    int n = rand() % 100;
    n = n * n * n;
    return n / 100;
}