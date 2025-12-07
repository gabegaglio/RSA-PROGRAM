#include "bigint.h"
#include <iostream>

namespace GMPUtils {

    void init(mpz_t num) {
        mpz_init(num);
    }

    void clear(mpz_t num) {
        mpz_clear(num);
    }

    void set(mpz_t dest, long long value) {
        mpz_set_si(dest, value);
    }

    void set(mpz_t dest, const std::string& str) {
        mpz_set_str(dest, str.c_str(), 10);
    }

    void copy(mpz_t dest, const mpz_t src) {
        mpz_set(dest, src);
    }

    std::string toString(const mpz_t num) {
        char* str = mpz_get_str(NULL, 10, num);
        std::string result(str);
        free(str);
        return result;
    }

    size_t numDigits(const mpz_t num) {
        return mpz_sizeinbase(num, 10);
    }

    void multiply(mpz_t result, const mpz_t a, const mpz_t b) {
        mpz_mul(result, a, b);
    }

    void subtract(mpz_t result, const mpz_t a, const mpz_t b) {
        mpz_sub(result, a, b);
    }

    void modExp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod) {
        mpz_powm(result, base, exp, mod);
    }

    bool modInverse(mpz_t result, const mpz_t a, const mpz_t m) {
        return mpz_invert(result, a, m) != 0;
    }

    void gcd(mpz_t result, const mpz_t a, const mpz_t b) {
        mpz_gcd(result, a, b);
    }

    int compare(const mpz_t a, const mpz_t b) {
        return mpz_cmp(a, b);
    }

    void randInit(gmp_randstate_t state, unsigned long seed) {
        gmp_randinit_default(state);
        gmp_randseed_ui(state, seed);
    }

    void randClear(gmp_randstate_t state) {
        gmp_randclear(state);
    }

    void randOddDigits(mpz_t result, int digits, gmp_randstate_t state) {
        mpz_t min, max, range;
        init(min);
        init(max);
        init(range);

        mpz_ui_pow_ui(min, 10, digits - 1);
        mpz_ui_pow_ui(max, 10, digits);
        mpz_sub_ui(max, max, 1);
        mpz_sub(range, max, min);
        mpz_add_ui(range, range, 1);
        mpz_urandomm(result, state, range);
        mpz_add(result, result, min);
        if (mpz_even_p(result)) {
            mpz_add_ui(result, result, 1);
        }

        clear(min);
        clear(max);
        clear(range);
    }

    bool isOdd(const mpz_t num) {
        return mpz_odd_p(num) != 0;
    }

    bool isDivisibleBy(const mpz_t num, unsigned long divisor) {
        return mpz_divisible_ui_p(num, divisor) != 0;
    }

}