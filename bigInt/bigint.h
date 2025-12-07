#ifndef BIGINT_H
#define BIGINT_H

#include<gmp.h>
#include<string>


namespace GMPUtils {
    void init(mpz_t num);
    void clear(mpz_t num);
    void set(mpz_t dest, long long value);
    void set(mpz_t dest, const std::string& str);
    void copy(mpz_t dest, const mpz_t src);
    std::string toString(const mpz_t num);
    size_t numDigits(const mpz_t num);
    void multiply(mpz_t result, const mpz_t a, const mpz_t b);
    void subtract(mpz_t result, const mpz_t a, const mpz_t b);
    void modExp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod);
    bool modInverse(mpz_t result, const mpz_t a, const mpz_t m);
    void gcd(mpz_t result, const mpz_t a, const mpz_t b);
    int compare(const mpz_t a, const mpz_t b);
    void randInit(gmp_randstate_t state, unsigned long seed);
    void randClear(gmp_randstate_t state);
    void randOddDigits(mpz_t result, int digits, gmp_randstate_t state);
    bool isOdd(const mpz_t num);
    bool isDivisibleBy(const mpz_t num, unsigned long divisor);
}


#endif