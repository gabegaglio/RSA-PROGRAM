#include "prim.h"
#include <iostream>

namespace Primality {

    int jacobi(const mpz_t a, const mpz_t n) {

        return mpz_jacobi(a, n);

    }

    bool solovay(const mpz_t n, int iterations, gmp_randstate_t state) {

        // check if n < 2
        if (mpz_cmp_ui(n, 2) < 0) return false;
        // check if n == 2
        if (mpz_cmp_ui(n, 2) == 0) return true;
        // check if n is even
        if (mpz_even_p(n)) return false;

        mpz_t a, gcd, jacobi_val, euler_val, n_minus_1, exponent;
        mpz_init(a);
        mpz_init(gcd);
        mpz_init(jacobi_val);
        mpz_init(euler_val);
        mpz_init(n_minus_1);
        mpz_init(exponent);

        mpz_sub_ui(n_minus_1, n, 1);

        // perform iterations rounds of testing
        for (int i = 0; i < iterations; i++) {
        // pick random a in range [2, n-1]
        mpz_t range;
        mpz_init(range);
        mpz_sub_ui(range, n, 3); // n - 2 - 1
        mpz_urandomm(a, state, range);
        mpz_add_ui(a, a, 2); // add 2 to get range [2, n-1]
        mpz_clear(range);

        // check gcd(a, n) = 1
        mpz_gcd(gcd, a, n);
        if (mpz_cmp_ui(gcd, 1) != 0) {
            // found a factor, definitely composite
            mpz_clear(a);
            mpz_clear(gcd);
            mpz_clear(jacobi_val);
            mpz_clear(euler_val);
            mpz_clear(n_minus_1);
            mpz_clear(exponent);
            return false;
        }

        // compute jacobi symbol J(a, n)
        int jacobi = jacobi(a, n);

        // convert jacobi symbol to mpz_t for comparison
        if (jacobi == -1) {
            mpz_set(jacobi_val, n_minus_1); // -1 ≡ n-1 (mod n)
        } else {
            mpz_set_si(jacobi_val, jacobi);
        }

        // compute a^((n-1)/2) mod n
        mpz_divexact_ui(exponent, n_minus_1, 2); // (n-1)/2
        mpz_powm(euler_val, a, exponent, n);

        if (mpz_cmp(jacobi_val, euler_val) != 0) {
            // test failed, definitely composite
            mpz_clear(a);
            mpz_clear(gcd);
            mpz_clear(jacobi_val);
            mpz_clear(euler_val);
            mpz_clear(n_minus_1);
            mpz_clear(exponent);
            return false;
        }
    }

    // passed all iterations, probably prime
    mpz_clear(a);
    mpz_clear(gcd);
    mpz_clear(jacobi_val);
    mpz_clear(euler_val);
    mpz_clear(n_minus_1);
    mpz_clear(exponent);
    return true;

}

}






