#ifndef PRIM_H
#define PRIM_H

#include <gmp.h>

namespace Primality {
    int jacobi(const mpz_t a, const mpz_t n);
    bool solovay(const mpz_t n, int iterations, gmp_randstate_t state);
}

#endif