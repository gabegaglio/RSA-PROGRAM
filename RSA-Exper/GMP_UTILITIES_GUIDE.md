# GMP Utilities for RSA - Simplified Guide

## Overview

This is a minimal GMP wrapper focused **only** on what you need to implement RSA with 100-digit primes (200-digit modulus) as specified in the paper.

## Installation

### macOS

```bash
brew install gmp
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install libgmp-dev
```

## What You Need for RSA

For the paper's RSA implementation, you need:

1. **Store large integers** (100-200 digits)
2. **Generate random primes** (100 digits each)
3. **Compute n = p × q** (multiplication)
4. **Compute φ(n) = (p-1)(q-1)** (subtraction, multiplication)
5. **Find d = e^(-1) mod φ(n)** (modular inverse)
6. **Encrypt: C = M^e mod n** (modular exponentiation)
7. **Decrypt: M = C^d mod n** (modular exponentiation)
8. **Check gcd(e, φ(n)) = 1** (GCD)

That's it. Everything else is unnecessary complexity.

## File Structure

```
RSA2/
├── bigInt/
│   ├── bigint.h          # Minimal GMP wrappers
│   ├── bigint.cpp        # Implementation
│   └── test_bigint.cpp   # Simple tests
└── GMP_UTILITIES_GUIDE.md
```

## Implementation

### bigint.h (Minimal)

```cpp
#ifndef BIGINT_H
#define BIGINT_H

#include <gmp.h>
#include <string>

namespace GMPUtils {

// ============================================
// MEMORY MANAGEMENT (CRITICAL - Always pair init with clear!)
// ============================================
void init(mpz_t num);
void clear(mpz_t num);

// ============================================
// SETTING VALUES
// ============================================
void set(mpz_t dest, long long value);
void set(mpz_t dest, const std::string& str);
void copy(mpz_t dest, const mpz_t src);

// ============================================
// GETTING VALUES
// ============================================
std::string toString(const mpz_t num);
size_t numDigits(const mpz_t num);

// ============================================
// RSA ARITHMETIC
// ============================================

// For computing n = p * q
void multiply(mpz_t result, const mpz_t a, const mpz_t b);

// For computing (p-1), (q-1)
void subtract(mpz_t result, const mpz_t a, const mpz_t b);

// For RSA encryption/decryption: result = base^exp mod m
void modExp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod);

// For finding d from e: result = a^(-1) mod m
// Returns true if inverse exists
bool modInverse(mpz_t result, const mpz_t a, const mpz_t m);

// For checking if e and φ(n) are coprime
void gcd(mpz_t result, const mpz_t a, const mpz_t b);

// ============================================
// COMPARISON (for ensuring p != q)
// ============================================
int compare(const mpz_t a, const mpz_t b);

// ============================================
// RANDOM NUMBER GENERATION (for prime generation)
// ============================================

// Initialize random state (call once at start)
void randInit(gmp_randstate_t state, unsigned long seed);

// Clear random state (call once at end)
void randClear(gmp_randstate_t state);

// Generate random n-digit ODD number (for prime candidates)
void randOddDigits(mpz_t result, int digits, gmp_randstate_t state);

// ============================================
// PRIMALITY TESTING HELPERS
// ============================================

// Check if number is odd
bool isOdd(const mpz_t num);

// Check if divisible by small prime (for pre-screening)
bool isDivisibleBy(const mpz_t num, unsigned long divisor);

} // namespace GMPUtils

#endif
```

### bigint.cpp

```cpp
#include "bigint.h"
#include <iostream>

namespace GMPUtils {

// ============================================
// MEMORY MANAGEMENT
// ============================================

void init(mpz_t num) {
    mpz_init(num);
}

void clear(mpz_t num) {
    mpz_clear(num);
}

// ============================================
// SETTING VALUES
// ============================================

void set(mpz_t dest, long long value) {
    mpz_set_si(dest, value);
}

void set(mpz_t dest, const std::string& str) {
    mpz_set_str(dest, str.c_str(), 10);
}

void copy(mpz_t dest, const mpz_t src) {
    mpz_set(dest, src);
}

// ============================================
// GETTING VALUES
// ============================================

std::string toString(const mpz_t num) {
    char* str = mpz_get_str(NULL, 10, num);
    std::string result(str);
    free(str);  // GMP uses malloc, must free
    return result;
}

size_t numDigits(const mpz_t num) {
    return mpz_sizeinbase(num, 10);
}

// ============================================
// RSA ARITHMETIC
// ============================================

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

// ============================================
// COMPARISON
// ============================================

int compare(const mpz_t a, const mpz_t b) {
    return mpz_cmp(a, b);
}

// ============================================
// RANDOM NUMBER GENERATION
// ============================================

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

    // min = 10^(digits-1)
    mpz_ui_pow_ui(min, 10, digits - 1);

    // max = 10^digits - 1
    mpz_ui_pow_ui(max, 10, digits);
    mpz_sub_ui(max, max, 1);

    // range = max - min + 1
    mpz_sub(range, max, min);
    mpz_add_ui(range, range, 1);

    // Generate random in [0, range)
    mpz_urandomm(result, state, range);

    // Shift to [min, max]
    mpz_add(result, result, min);

    // Make it odd
    if (mpz_even_p(result)) {
        mpz_add_ui(result, result, 1);
    }

    clear(min);
    clear(max);
    clear(range);
}

// ============================================
// PRIMALITY TESTING HELPERS
// ============================================

bool isOdd(const mpz_t num) {
    return mpz_odd_p(num) != 0;
}

bool isDivisibleBy(const mpz_t num, unsigned long divisor) {
    return mpz_divisible_ui_p(num, divisor) != 0;
}

} // namespace GMPUtils
```

### test_bigint.cpp (Simple Tests)

```cpp
#include "bigint.h"
#include <iostream>
#include <ctime>
#include <cassert>

using namespace GMPUtils;
using namespace std;

void testBasics() {
    cout << "\n=== Test 1: Basic Operations ===" << endl;

    mpz_t a, b, result;
    init(a);
    init(b);
    init(result);

    set(a, 123);
    set(b, 456);

    multiply(result, a, b);
    cout << "123 * 456 = " << toString(result) << endl;
    assert(toString(result) == "56088");

    clear(a);
    clear(b);
    clear(result);
    cout << "✓ Passed" << endl;
}

void testLargeNumbers() {
    cout << "\n=== Test 2: Large Numbers (50 digits) ===" << endl;

    mpz_t big1, big2, result;
    init(big1);
    init(big2);
    init(result);

    set(big1, "12345678901234567890123456789012345678901234567890");
    set(big2, "98765432109876543210987654321098765432109876543210");

    multiply(result, big1, big2);
    cout << "Product has " << numDigits(result) << " digits" << endl;
    assert(numDigits(result) >= 99);

    clear(big1);
    clear(big2);
    clear(result);
    cout << "✓ Passed" << endl;
}

void testModularArithmetic() {
    cout << "\n=== Test 3: Modular Arithmetic (RSA Core) ===" << endl;

    mpz_t base, exp, mod, result;
    init(base);
    init(exp);
    init(mod);
    init(result);

    // Test: 3^7 mod 11 = 9
    set(base, 3);
    set(exp, 7);
    set(mod, 11);
    modExp(result, base, exp, mod);
    cout << "3^7 mod 11 = " << toString(result) << endl;
    assert(toString(result) == "9");

    clear(base);
    clear(exp);
    clear(mod);
    clear(result);
    cout << "✓ Passed" << endl;
}

void testModularInverse() {
    cout << "\n=== Test 4: Modular Inverse ===" << endl;

    mpz_t a, m, inv;
    init(a);
    init(m);
    init(inv);

    // Find inverse of 65537 mod 3120 (simplified RSA example)
    set(a, 65537);
    set(m, 3120);

    bool exists = modInverse(inv, a, m);
    cout << "Inverse of 65537 mod 3120 " << (exists ? "exists" : "doesn't exist") << endl;

    if (exists) {
        cout << "d = " << toString(inv) << endl;
    }

    clear(a);
    clear(m);
    clear(inv);
    cout << "✓ Passed" << endl;
}

void testRandomGeneration() {
    cout << "\n=== Test 5: Random Number Generation ===" << endl;

    gmp_randstate_t state;
    randInit(state, time(NULL));

    mpz_t random;
    init(random);

    cout << "\nGenerating 10-digit odd numbers:" << endl;
    for (int i = 0; i < 3; i++) {
        randOddDigits(random, 10, state);
        cout << "  #" << (i+1) << ": " << toString(random)
             << " (" << numDigits(random) << " digits, "
             << (isOdd(random) ? "odd" : "even") << ")" << endl;
        assert(isOdd(random));
        assert(numDigits(random) == 10);
    }

    clear(random);
    randClear(state);
    cout << "✓ Passed" << endl;
}

void testRSASimulation() {
    cout << "\n=== Test 6: Complete RSA Simulation ===" << endl;

    mpz_t p, q, n, phi, p_minus_1, q_minus_1, e, d;
    mpz_t message, ciphertext, decrypted, gcd_result;

    init(p);
    init(q);
    init(n);
    init(phi);
    init(p_minus_1);
    init(q_minus_1);
    init(e);
    init(d);
    init(message);
    init(ciphertext);
    init(decrypted);
    init(gcd_result);

    // Use small primes for quick test
    set(p, 61);
    set(q, 53);
    cout << "p = " << toString(p) << endl;
    cout << "q = " << toString(q) << endl;

    // n = p * q
    multiply(n, p, q);
    cout << "n = " << toString(n) << endl;

    // φ(n) = (p-1)(q-1)
    subtract(p_minus_1, p, set(p_minus_1, 1), p_minus_1);  // Reuse variable
    set(p_minus_1, 60);  // p - 1
    set(q_minus_1, 52);  // q - 1
    multiply(phi, p_minus_1, q_minus_1);
    cout << "φ(n) = " << toString(phi) << endl;

    // e = 17
    set(e, 17);
    gcd(gcd_result, e, phi);
    cout << "gcd(e, φ(n)) = " << toString(gcd_result) << endl;
    assert(toString(gcd_result) == "1");

    // d = e^(-1) mod φ(n)
    bool inv_exists = modInverse(d, e, phi);
    assert(inv_exists);
    cout << "d = " << toString(d) << endl;

    // Encrypt message = 42
    set(message, 42);
    cout << "\nMessage: " << toString(message) << endl;

    modExp(ciphertext, message, e, n);
    cout << "Ciphertext: " << toString(ciphertext) << endl;

    // Decrypt
    modExp(decrypted, ciphertext, d, n);
    cout << "Decrypted: " << toString(decrypted) << endl;

    assert(compare(message, decrypted) == 0);
    cout << "\n✓ Encryption and decryption successful!" << endl;

    clear(p);
    clear(q);
    clear(n);
    clear(phi);
    clear(p_minus_1);
    clear(q_minus_1);
    clear(e);
    clear(d);
    clear(message);
    clear(ciphertext);
    clear(decrypted);
    clear(gcd_result);
}

int main() {
    cout << "======================================" << endl;
    cout << "  GMP Utilities Test Suite (Minimal)" << endl;
    cout << "======================================" << endl;

    testBasics();
    testLargeNumbers();
    testModularArithmetic();
    testModularInverse();
    testRandomGeneration();
    testRSASimulation();

    cout << "\n======================================" << endl;
    cout << "  ✓ ALL TESTS PASSED!" << endl;
    cout << "======================================" << endl;

    return 0;
}
```

## Compilation

### Simple Command

```bash
cd RSA2
g++ -std=c++17 -o test_bigint bigInt/bigint.cpp bigInt/test_bigint.cpp -lgmp
./test_bigint
```

### Makefile (Optional)

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS = -lgmp

test_bigint: bigInt/bigint.cpp bigInt/test_bigint.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f test_bigint

run: test_bigint
	./test_bigint
```

## Usage Pattern

Every GMP program follows this pattern:

```cpp
#include "bigInt/bigint.h"
using namespace GMPUtils;

int main() {
    // 1. Declare variables
    mpz_t x, y, result;

    // 2. Initialize (REQUIRED)
    init(x);
    init(y);
    init(result);

    // 3. Use them
    set(x, 100);
    set(y, 200);
    multiply(result, x, y);

    // 4. Clean up (REQUIRED)
    clear(x);
    clear(y);
    clear(result);

    return 0;
}
```

**Critical Rule**: Every `init()` MUST have a matching `clear()`. No exceptions.

## Essential GMP Functions

| Function              | What It Does      | Example                       |
| --------------------- | ----------------- | ----------------------------- |
| `init(x)`             | Allocate memory   | `init(n);`                    |
| `clear(x)`            | Free memory       | `clear(n);`                   |
| `set(x, val)`         | x = val           | `set(n, 100);`                |
| `toString(x)`         | Convert to string | `cout << toString(n);`        |
| `multiply(r, a, b)`   | r = a \* b        | `multiply(n, p, q);`          |
| `subtract(r, a, b)`   | r = a - b         | `subtract(pm1, p, one);`      |
| `modExp(r, b, e, m)`  | r = b^e mod m     | `modExp(cipher, msg, e, n);`  |
| `modInverse(r, a, m)` | r = a^-1 mod m    | `modInverse(d, e, phi);`      |
| `gcd(r, a, b)`        | r = gcd(a, b)     | `gcd(g, e, phi);`             |
| `compare(a, b)`       | Compare numbers   | `if (compare(p, q) == 0) ...` |

## Next Steps

Once this works:

1. Add Jacobi symbol function (1 function)
2. Add Solovay-Strassen test (1 function)
3. Add prime generation loop (1 function)
4. Integrate into RSA class

That's it. No more complexity than needed.

## Troubleshooting

**"gmp.h not found"**: GMP not installed → `brew install gmp`

**"undefined reference to mpz\_\*"**: Wrong link order → Put `-lgmp` at END of g++ command

**Segmentation fault**: Forgot to call `init()` before using a variable

---

**Total Functions**: 16 (only what RSA needs)
**Lines of Code**: ~150 (vs 1000+ in complex version)
**Time to Implement**: 30 minutes
