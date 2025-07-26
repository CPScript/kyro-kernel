#include <stdint.h>

int abs(int x) {
    return x < 0 ? -x : x;
}

long labs(long x) {
    return x < 0 ? -x : x;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

uint32_t pow(uint32_t base, uint32_t exp) {
    uint32_t result = 1;
    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return result;
}

uint32_t sqrt(uint32_t x) {
    if (x == 0) return 0;
    
    uint32_t result = x;
    uint32_t prev;
    
    do {
        prev = result;
        result = (result + x / result) / 2;
    } while (result < prev);
    
    return prev;
}

uint32_t gcd(uint32_t a, uint32_t b) {
    while (b != 0) {
        uint32_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

uint32_t lcm(uint32_t a, uint32_t b) {
    return (a * b) / gcd(a, b);
}
