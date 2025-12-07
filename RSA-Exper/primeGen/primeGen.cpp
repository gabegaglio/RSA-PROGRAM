#include "primeGen.h"
#include <../bigInt/bigint.h>
#include <../primality/prim.h>
#include <iostream>

namespace PrimeGen {

    void genRanOdd(mpz_t result, int digits, gmp_randstate_t state) {
        GMPUtils::randOddDigits(result, digits, state);

        if (mpz_even_p(result)) {
            mpz_add_ui(result, result, 1);
        }
    }

    void genPrime(mpz_t result, int digits, int iterations, gmp_randstate_t state) {
        std::cout << "Generating " << digits << "-digit prime..." << std::endl;

        int attempts = 0;
        bool found = false;

        while (!found) {
            attempts++;
            genRanOdd(result, digits, state);

            bool divisible = false;
            int small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
            for (int p : small_primes) {
                if (mpz_divisible_ui_p(result, p)) {
                    if (mpz_cmp_ui(result, p) != 0) {
                        divisible = true;
                        break;
                    }
                }
            }

            if (divisible) {
                continue;
            }

            if (Primality::solovay(result, iterations, state)) {
                found = true;
                std::cout << "Found prime after " << attempts << " attempts." << std::endl;
                std::cout << "Expected attempts (by Prime Number Theorem): ~" << (int)(2.303 * digits / 2) << std::endl;
            }

            if (attempts % 10 == 0) {
                std::cout << "Tested " << attempts << " candidates..." << std::endl;
            }
        }
    }
}