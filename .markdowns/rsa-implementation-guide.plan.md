<!-- 79d9918f-f5ee-4f53-836f-668ced2c7140 51a4afa1-9b82-4bde-a0a2-f64632eb71ec -->

# RSA System Implementation Guide

## Overview

Build a complete RSA cryptography system that integrates all your existing algorithms into a cohesive program for key generation, encryption, and decryption.

## Architecture

### How Your Algorithms Work Together

```
┌─────────────────────────────────────────┐
│         RSA KEY GENERATION              │
├─────────────────────────────────────────┤
│ 1. Sieve → Generate prime candidates    │
│ 2. isPrime() → Verify p and q are prime │
│ 3. Compute n = p × q                    │
│ 4. Compute φ(n) = (p-1)(q-1)           │
│ 5. Choose e (public exponent)           │
│ 6. Extended Euclidean → Find d = e⁻¹    │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│         ENCRYPTION                      │
├─────────────────────────────────────────┤
│ modExp(message, e, n) → ciphertext     │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│         DECRYPTION                      │
├─────────────────────────────────────────┤
│ modExp(ciphertext, d, n) → message     │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│    ATTACK (Future Enhancement)          │
├─────────────────────────────────────────┤
│ Fermat Factorization → Factor weak n    │
│ Visualize the attack process            │
└─────────────────────────────────────────┘
```

## Phase 1: Basic RSA System (Implement Now)

### Step 1: Automatic Key Generation Function

Create a function that automatically generates RSA keys:

```cpp
struct RSAKeys {
    long long n;      // modulus
    long long e;      // public exponent
    long long d;      // private exponent
    long long p, q;   // primes (keep for demonstration)
};

RSAKeys generateKeys() {
    RSAKeys keys;

    // Step 1: Generate prime candidates using sieve
    // For demonstration, use primes between 50-200
    vector<long long> primes = sieve(200);

    // Step 2: Select two random primes from the middle of the list
    // (avoid very small primes for security, avoid largest for variety)
    int mid = primes.size() / 2;
    long long p = primes[mid];      // e.g., ~100th prime
    long long q = primes[mid + 5];  // e.g., ~105th prime (different from p)

    keys.p = p;
    keys.q = q;

    // Step 3: Compute n and φ(n)
    keys.n = p * q;
    long long phi = (p - 1) * (q - 1);

    // Step 4: Choose public exponent e
    keys.e = 65537;  // Common choice
    if (keys.e >= phi || euclidean(keys.e, phi) != 1) {
        keys.e = 3;
        while (euclidean(keys.e, phi) != 1) keys.e += 2;
    }

    // Step 5: Find private exponent d using Extended Euclidean
    vector<vector<long long>> ext = extendedEuclidean(keys.e, phi);
    keys.d = ext[ext.size() - 2][2];  // Get Bezout coefficient
    if (keys.d < 0) keys.d += phi;

    return keys;
}
```

**Alternative: Let user choose prime size**

```cpp
RSAKeys generateKeys(int maxPrime = 200) {
    // User can optionally specify difficulty
    // Larger primes = stronger encryption, larger n
}
```

### Step 2: String to Number Conversion

RSA works on numbers, so we need to convert strings to numbers:

```cpp
// Convert a single character to a number (ASCII value)
long long charToNum(char c) {
    return (long long)c;
}

// Convert string to vector of numbers (one per character)
vector<long long> stringToNumbers(string message) {
    vector<long long> nums;
    for (char c : message) {
        nums.push_back((long long)c);
    }
    return nums;
}

// Convert vector of numbers back to string
string numbersToString(vector<long long> nums) {
    string result = "";
    for (long long num : nums) {
        result += (char)num;
    }
    return result;
}
```

**Alternative (advanced): Block cipher mode**

Pack multiple characters into one number if n is large enough:

```cpp
// Example: "HI" -> 72*256 + 73 = 18505
long long packChars(string block, long long n) {
    long long packed = 0;
    for (char c : block) {
        packed = packed * 256 + (long long)c;
        if (packed >= n) break;  // Too big, stop
    }
    return packed;
}
```

### Step 3: Encryption Function

```cpp
// Encrypt a string (character by character)
// Works for any input: "HELLO", "123", "Hi there!"
vector<long long> encryptString(string message, long long e, long long n) {
    vector<long long> ciphertexts;

    for (char c : message) {
        long long num = (long long)c;  // Convert char to ASCII value
        long long cipher = modExp(num, e, n);  // Encrypt using modExp
        ciphertexts.push_back(cipher);
    }

    return ciphertexts;
}
```

### Step 4: Decryption Function

```cpp
// Decrypt a vector of ciphertext numbers back to string
string decryptString(vector<long long> ciphertexts, long long d, long long n) {
    string result = "";

    for (long long cipher : ciphertexts) {
        long long num = modExp(cipher, d, n);  // Decrypt using modExp
        result += (char)num;  // Convert ASCII value back to character
    }

    return result;
}
```

### Step 5: Complete Main Program Structure

```cpp
#include <iostream>
#include <vector>
#include <string>
#include "euclidean/euclidean.h"
#include "sieve/sieve.h"
#include "uft/uft.h"
#include "mod_exponentiation/modexp.h"

using namespace std;

// Global variables to store state between menu choices
RSAKeys keys;
bool hasKeys = false;
vector<long long> storedCiphertext;  // Store encrypted message

int main() {
    while (true) {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║     RSA Cryptography System           ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "1. Generate RSA Keys" << endl;
        cout << "2. Encrypt Message" << endl;
        cout << "3. Decrypt Message" << endl;
        cout << "4. Display Current Keys" << endl;
        cout << "0. Exit" << endl;
        cout << "\nChoice: ";

        int choice;
        cin >> choice;

        switch(choice) {
            case 1: {  // Generate Keys
                cout << "\nGenerating RSA keys..." << endl;
                keys = generateKeys();
                hasKeys = true;

                cout << "✓ Keys generated successfully!" << endl;
                cout << "\nPublic Key:  (e=" << keys.e << ", n=" << keys.n << ")" << endl;
                cout << "Private Key: (d=" << keys.d << ", n=" << keys.n << ")" << endl;
                cout << "Using primes: p=" << keys.p << ", q=" << keys.q << endl;
                break;
            }

            case 2: {  // Encrypt Message
                if (!hasKeys) {
                    cout << "⚠ Please generate keys first (Option 1)" << endl;
                    break;
                }

                string message;
                cout << "\nEnter message to encrypt: ";
                cin.ignore();
                getline(cin, message);

                // Encrypt the string
                storedCiphertext = encryptString(message, keys.e, keys.n);

                cout << "\n✓ Message encrypted!" << endl;
                cout << "Ciphertext: ";
                for (long long c : storedCiphertext) {
                    cout << c << " ";
                }
                cout << endl;
                break;
            }

            case 3: {  // Decrypt Message
                if (!hasKeys) {
                    cout << "⚠ Please generate keys first (Option 1)" << endl;
                    break;
                }

                if (storedCiphertext.empty()) {
                    cout << "⚠ No encrypted message found. Encrypt a message first (Option 2)" << endl;
                    break;
                }

                // Decrypt the stored ciphertext
                string decrypted = decryptString(storedCiphertext, keys.d, keys.n);

                cout << "\n✓ Message decrypted!" << endl;
                cout << "Decrypted message: " << decrypted << endl;
                break;
            }

            case 4: {  // Display Keys
                if (!hasKeys) {
                    cout << "⚠ No keys generated yet" << endl;
                    break;
                }

                cout << "\n=== Current RSA Keys ===" << endl;
                cout << "Primes used: p=" << keys.p << ", q=" << keys.q << endl;
                cout << "Modulus: n = p × q = " << keys.n << endl;
                cout << "φ(n) = (p-1)(q-1) = " << (keys.p - 1) * (keys.q - 1) << endl;
                cout << "\nPublic Key:  (e=" << keys.e << ", n=" << keys.n << ")" << endl;
                cout << "Private Key: (d=" << keys.d << ", n=" << keys.n << ")" << endl;
                break;
            }

            case 0:
                cout << "Goodbye!" << endl;
                return 0;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
```

**Key Points:**

- **Global variables**: `keys`, `hasKeys`, `storedCiphertext` persist between menu selections
- **Error checking**: Each option checks if prerequisites are met (keys generated, message encrypted)
- **User-friendly**: Clear prompts and status messages with symbols (✓, ⚠)
- **Automatic flow**: Keys auto-generated, ciphertext auto-stored, no manual copying needed

## Phase 2: Visual Decryption (Future)

Add visual step-by-step decryption showing:

- The modular exponentiation process
- Binary representation of exponent
- Each squaring and multiplication step
- Running result at each iteration

```cpp
void visualDecrypt(long long ciphertext, long long d, long long n) {
    cout << "\n=== Visual Decryption ===" << endl;
    cout << "Computing: " << ciphertext << "^" << d << " mod " << n << endl;

    // Show binary of exponent
    // Step through modExp algorithm with output at each step
    // Display intermediate results
}
```

## Phase 3: Attack Weak Keys (Future)

Add attack demonstration using Fermat factorization:

```cpp
void attackWeakKey(long long n) {
    cout << "\n=== Attacking RSA Key ===" << endl;
    cout << "Attempting Fermat Factorization on n = " << n << endl;

    vector<vector<long long>> attempts = fermatFactorization(n);

    // Visual display of attack progress
    // Show each (x, y) pair tested
    // When factors found, compute private key
}
```

## Compilation

```bash
cd /Users/gabe/coding/general_projects/misc_programs/RSA_Programs

g++ -std=c++17 main.cpp \
    euclidean/euclidean.cpp \
    sieve/sieve.cpp \
    uft/uft.cpp \
    mod_exponentiation/modexp.cpp \
    fermatFactorization/fermat.cpp \
    quotrem/quotrem.cpp \
    -o rsa_system

./rsa_system
```

## Testing Strategy

1. **Test with small primes**: p=61, q=53

   - Easy to verify manually
   - n=3233, φ(n)=3120

2. **Test encryption/decryption**:

   - Encrypt message: 123
   - Decrypt result
   - Should get 123 back

3. **Test weak keys** (for attack demo):

   - Choose p and q that are close together
   - Fermat factorization will succeed quickly

## Key Files to Modify

- `main.cpp` - Primary implementation file
- All existing .cpp files remain unchanged (already have `main()` functions for standalone testing)

## Notes

- Each individual algorithm can still run standalone
- The unified `main.cpp` brings them all together
- Keep primes small initially (< 1000) for easier debugging
- For larger primes, consider using Miller-Rabin test (future enhancement)

### To-dos

- [ ] Define RSAKeys struct to hold n, e, d, p, q
- [ ] Implement generateKeys() function using isPrime, euclidean, and extendedEuclidean
- [ ] Implement encrypt() and decrypt() wrapper functions using modExp
- [ ] Create main menu loop with 4 options: generate keys, encrypt, decrypt, display keys
- [ ] Test complete system with small primes (p=61, q=53)
- [ ] Add visual step-by-step decryption display (Phase 2)
- [ ] Implement attack demonstration using Fermat factorization (Phase 3)
