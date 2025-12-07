#ifndef PRIMEGEN_H
#define PRIMEGEN_H

#include <gmp.h>

namespace PrimeGen {

    void generatePrime(mpz_t result, int digits, int iterations, gmp_randstate_t state);

    // Generate random odd number with n digits
    void generateRandomOdd(mpz_t result, int digits, gmp_randstate_t state);
}

#endif