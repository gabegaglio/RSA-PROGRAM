# Large Prime Generation with Solovay-Strassen Primality Test

## Overview

This guide shows how to implement the Solovay-Strassen probabilistic primality test to generate large (100-digit) primes for RSA encryption using the GMP (GNU Multiple Precision) library.

## Architecture

```
RSA2/
├── bigInt/
│   ├── bigint.h          # GMP wrapper and utilities
│   └── bigint.cpp        # Implementation
├── primality/
│   ├── primality.h       # Primality testing functions
│   └── primality.cpp     # Solovay-Strassen implementation
├── primeGen/
│   ├── primeGen.h        # Prime generation
│   └── primeGen.cpp      # Random prime generation logic
└── main.cpp              # RSA system
```

## Step 1: GMP Setup and Wrapper

### bigint.h

```cpp
#ifndef BIGINT_H
#define BIGINT_H

#include <gmp.h>
#include <string>
#include <iostream>

// Wrapper class for GMP operations (optional - can use GMP directly)
class BigInt {
private:
    mpz_t value;

public:
    BigInt();
    BigInt(const std::string& str, int base = 10);
    BigInt(long long num);
    BigInt(const BigInt& other);
    ~BigInt();

    // Assignment
    BigInt& operator=(const BigInt& other);

    // Arithmetic operations
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    // Comparison
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;

    // Access to raw GMP value
    mpz_t& getRaw() { return value; }
    const mpz_t& getRaw() const { return value; }

    // Utility functions
    std::string toString(int base = 10) const;
    void print() const;

    friend std::ostream& operator<<(std::ostream& os, const BigInt& num);
};

// Standalone GMP utility functions
namespace GMPUtils {
    // Modular exponentiation: (base^exp) mod m
    void modExp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod);

    // GCD
    void gcd(mpz_t result, const mpz_t a, const mpz_t b);

    // Generate random number in range [min, max]
    void randomRange(mpz_t result, const mpz_t min, const mpz_t max, gmp_randstate_t state);

    // Generate random n-digit number
    void randomNDigit(mpz_t result, int digits, gmp_randstate_t state);
}

#endif
```

### bigint.cpp (Key Functions)

```cpp
#include "bigint.h"
#include <ctime>

// Constructor implementations
BigInt::BigInt() {
    mpz_init(value);
}

BigInt::BigInt(const std::string& str, int base) {
    mpz_init_set_str(value, str.c_str(), base);
}

BigInt::BigInt(long long num) {
    mpz_init_set_si(value, num);
}

BigInt::~BigInt() {
    mpz_clear(value);
}

// Modular exponentiation
void GMPUtils::modExp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod) {
    mpz_powm(result, base, exp, mod);
}

// GCD using GMP
void GMPUtils::gcd(mpz_t result, const mpz_t a, const mpz_t b) {
    mpz_gcd(result, a, b);
}

// Generate random n-digit number
void GMPUtils::randomNDigit(mpz_t result, int digits, gmp_randstate_t state) {
    // Generate number with exactly n digits
    // Minimum n-digit number: 10^(n-1)
    // Maximum n-digit number: 10^n - 1

    mpz_t min, max, range;
    mpz_init(min);
    mpz_init(max);
    mpz_init(range);

    // min = 10^(digits-1)
    mpz_ui_pow_ui(min, 10, digits - 1);

    // max = 10^digits - 1
    mpz_ui_pow_ui(max, 10, digits);
    mpz_sub_ui(max, max, 1);

    // range = max - min + 1
    mpz_sub(range, max, min);
    mpz_add_ui(range, range, 1);

    // Generate random in range
    mpz_urandomm(result, state, range);
    mpz_add(result, result, min);

    mpz_clear(min);
    mpz_clear(max);
    mpz_clear(range);
}
```

## Step 2: Jacobi Symbol Computation

The Jacobi symbol J(a,b) is required for the Solovay-Strassen test.

### primality.h

```cpp
#ifndef PRIMALITY_H
#define PRIMALITY_H

#include <gmp.h>

namespace Primality {
    // Compute Jacobi symbol (a/n)
    // Returns -1, 0, or 1
    int jacobiSymbol(const mpz_t a, const mpz_t n);

    // Solovay-Strassen primality test
    // iterations: number of test rounds (more = higher confidence)
    // Returns true if probably prime, false if composite
    bool solovayStrassen(const mpz_t n, int iterations, gmp_randstate_t state);

    // Miller-Rabin test (alternative/additional test)
    bool millerRabin(const mpz_t n, int iterations, gmp_randstate_t state);
}

#endif
```

### primality.cpp

```cpp
#include "primality.h"
#include <iostream>

namespace Primality {

// Jacobi symbol computation using the algorithm's properties
int jacobiSymbol(const mpz_t a_input, const mpz_t n_input) {
    // GMP has built-in Jacobi symbol!
    return mpz_jacobi(a_input, n_input);

    /* Manual implementation if needed:
    mpz_t a, n, temp;
    mpz_init_set(a, a_input);
    mpz_init_set(n, n_input);
    mpz_init(temp);

    int result = 1;

    while (mpz_cmp_ui(a, 0) != 0) {
        // Remove factors of 2 from a
        while (mpz_even_p(a)) {
            mpz_divexact_ui(a, a, 2);

            // If n ≡ 3 or 5 (mod 8), multiply result by -1
            mpz_mod_ui(temp, n, 8);
            unsigned long mod8 = mpz_get_ui(temp);
            if (mod8 == 3 || mod8 == 5) {
                result = -result;
            }
        }

        // Swap a and n (quadratic reciprocity)
        mpz_swap(a, n);

        // If both a and n ≡ 3 (mod 4), multiply result by -1
        mpz_mod_ui(temp, a, 4);
        unsigned long a_mod4 = mpz_get_ui(temp);
        mpz_mod_ui(temp, n, 4);
        unsigned long n_mod4 = mpz_get_ui(temp);

        if (a_mod4 == 3 && n_mod4 == 3) {
            result = -result;
        }

        // a = a mod n
        mpz_mod(a, a, n);
    }

    if (mpz_cmp_ui(n, 1) == 0) {
        mpz_clear(a);
        mpz_clear(n);
        mpz_clear(temp);
        return result;
    }

    mpz_clear(a);
    mpz_clear(n);
    mpz_clear(temp);
    return 0;
    */
}

// Solovay-Strassen primality test
bool solovayStrassen(const mpz_t n, int iterations, gmp_randstate_t state) {
    // Check if n < 2
    if (mpz_cmp_ui(n, 2) < 0) {
        return false;
    }

    // Check if n == 2
    if (mpz_cmp_ui(n, 2) == 0) {
        return true;
    }

    // Check if n is even
    if (mpz_even_p(n)) {
        return false;
    }

    mpz_t a, gcd_result, jacobi_val, euler_val, n_minus_1, exponent;
    mpz_init(a);
    mpz_init(gcd_result);
    mpz_init(jacobi_val);
    mpz_init(euler_val);
    mpz_init(n_minus_1);
    mpz_init(exponent);

    mpz_sub_ui(n_minus_1, n, 1);

    // Perform iterations rounds of testing
    for (int i = 0; i < iterations; i++) {
        // Pick random a in range [2, n-1]
        mpz_t range;
        mpz_init(range);
        mpz_sub_ui(range, n, 3);  // n - 2 - 1
        mpz_urandomm(a, state, range);
        mpz_add_ui(a, a, 2);  // Add 2 to get range [2, n-1]
        mpz_clear(range);

        // Check gcd(a, n) = 1
        mpz_gcd(gcd_result, a, n);
        if (mpz_cmp_ui(gcd_result, 1) != 0) {
            // Found a factor, definitely composite
            mpz_clear(a);
            mpz_clear(gcd_result);
            mpz_clear(jacobi_val);
            mpz_clear(euler_val);
            mpz_clear(n_minus_1);
            mpz_clear(exponent);
            return false;
        }

        // Compute Jacobi symbol J(a, n)
        int jacobi = jacobiSymbol(a, n);

        // Convert Jacobi symbol to mpz_t for comparison
        if (jacobi == -1) {
            mpz_set(jacobi_val, n_minus_1);  // -1 ≡ n-1 (mod n)
        } else {
            mpz_set_si(jacobi_val, jacobi);
        }

        // Compute a^((n-1)/2) mod n
        mpz_divexact_ui(exponent, n_minus_1, 2);  // (n-1)/2
        mpz_powm(euler_val, a, exponent, n);

        // Check if J(a,n) ≡ a^((n-1)/2) (mod n)
        if (mpz_cmp(jacobi_val, euler_val) != 0) {
            // Test failed, definitely composite
            mpz_clear(a);
            mpz_clear(gcd_result);
            mpz_clear(jacobi_val);
            mpz_clear(euler_val);
            mpz_clear(n_minus_1);
            mpz_clear(exponent);
            return false;
        }
    }

    // Passed all iterations, probably prime
    mpz_clear(a);
    mpz_clear(gcd_result);
    mpz_clear(jacobi_val);
    mpz_clear(euler_val);
    mpz_clear(n_minus_1);
    mpz_clear(exponent);
    return true;
}

} // namespace Primality
```

## Step 3: Prime Generation

### primeGen.h

```cpp
#ifndef PRIMEGEN_H
#define PRIMEGEN_H

#include <gmp.h>

namespace PrimeGen {
    // Generate a random prime with specified number of digits
    // digits: number of decimal digits (e.g., 100)
    // iterations: number of Solovay-Strassen test rounds (recommend 20-50)
    void generatePrime(mpz_t result, int digits, int iterations, gmp_randstate_t state);

    // Generate random odd number with n digits
    void generateRandomOdd(mpz_t result, int digits, gmp_randstate_t state);
}

#endif
```

### primeGen.cpp

```cpp
#include "primeGen.h"
#include "../bigInt/bigint.h"
#include "../primality/primality.h"
#include <iostream>

namespace PrimeGen {

void generateRandomOdd(mpz_t result, int digits, gmp_randstate_t state) {
    // Generate random n-digit number
    GMPUtils::randomNDigit(result, digits, state);

    // Make it odd
    if (mpz_even_p(result)) {
        mpz_add_ui(result, result, 1);
    }
}

void generatePrime(mpz_t result, int digits, int iterations, gmp_randstate_t state) {
    std::cout << "Generating " << digits << "-digit prime..." << std::endl;

    int attempts = 0;
    bool found = false;

    while (!found) {
        attempts++;

        // Generate random odd number
        generateRandomOdd(result, digits, state);

        // Quick check: divide by small primes first (optional optimization)
        // This saves time on obvious composites
        bool divisible = false;
        int small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
        for (int p : small_primes) {
            if (mpz_divisible_ui_p(result, p)) {
                if (mpz_cmp_ui(result, p) != 0) {  // Unless result == p
                    divisible = true;
                    break;
                }
            }
        }

        if (divisible) {
            continue;  // Skip primality test
        }

        // Run Solovay-Strassen test
        if (Primality::solovayStrassen(result, iterations, state)) {
            found = true;
            std::cout << "Found prime after " << attempts << " attempts." << std::endl;
            std::cout << "Expected attempts (by Prime Number Theorem): ~"
                      << (int)(2.303 * digits / 2) << std::endl;
        }

        if (attempts % 10 == 0) {
            std::cout << "Tested " << attempts << " candidates..." << std::endl;
        }
    }
}

} // namespace PrimeGen
```

## Step 4: Integration into RSA System

### Updated RSA Key Generation

```cpp
#include <gmp.h>
#include "bigInt/bigint.h"
#include "primality/primality.h"
#include "primeGen/primeGen.h"

class RSA {
private:
    struct Keys {
        mpz_t n;          // modulus
        mpz_t e;          // public exponent
        mpz_t d;          // private exponent
        mpz_t p, q;       // primes
        mpz_t phi;        // φ(n) = (p-1)(q-1)
    };

    Keys keys;
    gmp_randstate_t randState;
    bool hasKey = false;

    void initKeys() {
        mpz_init(keys.n);
        mpz_init(keys.e);
        mpz_init(keys.d);
        mpz_init(keys.p);
        mpz_init(keys.q);
        mpz_init(keys.phi);
    }

    void clearKeys() {
        mpz_clear(keys.n);
        mpz_clear(keys.e);
        mpz_clear(keys.d);
        mpz_clear(keys.p);
        mpz_clear(keys.q);
        mpz_clear(keys.phi);
    }

public:
    RSA() {
        initKeys();

        // Initialize random state
        gmp_randinit_default(randState);
        gmp_randseed_ui(randState, time(NULL));
    }

    ~RSA() {
        clearKeys();
        gmp_randclear(randState);
    }

    void generateKeys(int digitSize = 100, int primalityRounds = 40) {
        std::cout << "=== RSA Key Generation ===" << std::endl;
        std::cout << "Generating two " << digitSize << "-digit primes..." << std::endl;

        // Generate prime p
        std::cout << "\nGenerating prime p..." << std::endl;
        PrimeGen::generatePrime(keys.p, digitSize, primalityRounds, randState);

        // Generate prime q (different from p)
        std::cout << "\nGenerating prime q..." << std::endl;
        do {
            PrimeGen::generatePrime(keys.q, digitSize, primalityRounds, randState);
        } while (mpz_cmp(keys.p, keys.q) == 0);

        // Compute n = p * q
        mpz_mul(keys.n, keys.p, keys.q);
        std::cout << "\nn = p * q computed (" << mpz_sizeinbase(keys.n, 10)
                  << " digits)" << std::endl;

        // Compute φ(n) = (p-1)(q-1)
        mpz_t p_minus_1, q_minus_1;
        mpz_init(p_minus_1);
        mpz_init(q_minus_1);
        mpz_sub_ui(p_minus_1, keys.p, 1);
        mpz_sub_ui(q_minus_1, keys.q, 1);
        mpz_mul(keys.phi, p_minus_1, q_minus_1);
        mpz_clear(p_minus_1);
        mpz_clear(q_minus_1);

        // Choose e = 65537 (common choice, Fermat prime F4)
        mpz_set_ui(keys.e, 65537);

        // Verify gcd(e, φ(n)) = 1
        mpz_t gcd_result;
        mpz_init(gcd_result);
        mpz_gcd(gcd_result, keys.e, keys.phi);

        if (mpz_cmp_ui(gcd_result, 1) != 0) {
            // If e doesn't work, find another
            std::cout << "Warning: e=65537 doesn't work, finding alternative..." << std::endl;
            mpz_set_ui(keys.e, 3);
            while (mpz_cmp_ui(gcd_result, 1) != 0) {
                mpz_add_ui(keys.e, keys.e, 2);
                mpz_gcd(gcd_result, keys.e, keys.phi);
            }
        }
        mpz_clear(gcd_result);

        // Compute d = e^(-1) mod φ(n)
        // d is the modular multiplicative inverse of e mod φ(n)
        if (mpz_invert(keys.d, keys.e, keys.phi) == 0) {
            std::cerr << "Error: Could not compute modular inverse!" << std::endl;
            return;
        }

        hasKey = true;
        std::cout << "\n=== Keys Generated Successfully ===" << std::endl;
    }

    void displayKeys() {
        if (!hasKey) {
            std::cout << "No keys generated yet!" << std::endl;
            return;
        }

        std::cout << "\n=== RSA Keys ===" << std::endl;
        std::cout << "p: " << mpz_get_str(NULL, 10, keys.p) << std::endl;
        std::cout << "\nq: " << mpz_get_str(NULL, 10, keys.q) << std::endl;
        std::cout << "\nn: " << mpz_get_str(NULL, 10, keys.n) << std::endl;
        std::cout << "\nφ(n): " << mpz_get_str(NULL, 10, keys.phi) << std::endl;
        std::cout << "\nPublic exponent (e): " << mpz_get_str(NULL, 10, keys.e) << std::endl;
        std::cout << "\nPrivate exponent (d): " << mpz_get_str(NULL, 10, keys.d) << std::endl;
    }

    // Encrypt single block
    void encrypt(mpz_t ciphertext, const mpz_t plaintext) {
        mpz_powm(ciphertext, plaintext, keys.e, keys.n);
    }

    // Decrypt single block
    void decrypt(mpz_t plaintext, const mpz_t ciphertext) {
        mpz_powm(plaintext, ciphertext, keys.d, keys.n);
    }
};
```

## Step 5: Compilation

### Makefile

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lgmp

SOURCES = main.cpp bigInt/bigint.cpp primality/primality.cpp primeGen/primeGen.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = rsa_system

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
```

### Or Simple Compilation Command

```bash
g++ -std=c++17 -o rsa_system main.cpp bigInt/bigint.cpp primality/primality.cpp primeGen/primeGen.cpp -lgmp
```

## Step 6: Installing GMP

### macOS

```bash
brew install gmp
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install libgmp-dev
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install gmp-devel
```

## Testing Example

### main.cpp

```cpp
#include <iostream>
#include "bigInt/bigint.h"
#include "primality/primality.h"
#include "primeGen/primeGen.h"

int main() {
    // Initialize GMP random state
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    // Test 1: Generate small prime (10 digits)
    std::cout << "=== Test 1: Generate 10-digit prime ===" << std::endl;
    mpz_t small_prime;
    mpz_init(small_prime);
    PrimeGen::generatePrime(small_prime, 10, 20, state);
    std::cout << "Prime: " << mpz_get_str(NULL, 10, small_prime) << std::endl;

    // Test 2: Verify with Solovay-Strassen
    std::cout << "\n=== Test 2: Verify primality ===" << std::endl;
    bool isPrime = Primality::solovayStrassen(small_prime, 40, state);
    std::cout << "Is prime? " << (isPrime ? "Yes" : "No") << std::endl;

    mpz_clear(small_prime);

    // Test 3: Generate larger primes (start with 20, then 50, then 100)
    std::cout << "\n=== Test 3: Generate 20-digit prime ===" << std::endl;
    mpz_t larger_prime;
    mpz_init(larger_prime);
    PrimeGen::generatePrime(larger_prime, 20, 30, state);
    std::cout << "20-digit prime generated!" << std::endl;

    mpz_clear(larger_prime);
    gmp_randclear(state);

    return 0;
}
```

## Performance Notes

### Prime Number Theorem Expectations

For an n-digit prime:

- Average attempts needed ≈ ln(10^n) / 2 ≈ 1.15 \* n
- For 100-digit prime: ~115 attempts expected
- Each Solovay-Strassen test with 40 iterations is very reliable

### Optimization Tips

1. **Pre-screening**: Test divisibility by small primes (2, 3, 5, 7, ...) before running Solovay-Strassen
2. **Iterations**: 20-40 iterations gives very high confidence (error probability < 2^-40)
3. **Parallel generation**: Generate p and q in separate threads if needed
4. **Start small**: Test with 10-20 digit primes first, then scale up

## Security Considerations

1. **Random number generator**: Use cryptographically secure RNG for production
2. **Prime size**: 100-digit primes give 200-digit modulus (~664 bits) - consider 1024+ bits for modern security
3. **Prime distance**: Ensure |p - q| is large enough
4. **Public exponent**: e = 65537 is standard and secure

## References

- Solovay, R., & Strassen, V. (1977). "A fast Monte-Carlo test for primality"
- GMP Documentation: https://gmplib.org/manual/
- Prime Number Theorem for estimating prime density
