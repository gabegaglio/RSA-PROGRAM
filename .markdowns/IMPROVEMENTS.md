# RSA System - Future Improvements

## Current Implementation Status

✅ Basic RSA key generation working
✅ Character-by-character encryption/decryption
✅ All core algorithms integrated

## Known Security Weaknesses (Educational)

### 1. Deterministic Prime Selection

**Issue:** Keys are always the same (P=89, Q=109)

**Current Code:**

```cpp
int mid = primes.size() / 2;
long long p = primes[mid];      // Always picks same prime
long long q = primes[mid + 5];  // Always picks same prime
```

**Impact:** Predictable keys for testing, but not realistic

**Solution:** Add randomization

```cpp
#include <random>
#include <ctime>

// In genKeys():
srand(time(0));
int index1 = rand() % primes.size();
int index2 = rand() % primes.size();

while (index1 == index2) {
    index2 = rand() % primes.size();
}

long long p = primes[index1];
long long q = primes[index2];
```

**Priority:** Medium (improves realism, maintains testability option)

---

### 2. Character-by-Character Encryption (Pattern Leakage)

**Issue:** Same character → same ciphertext

**Current Code:**

```cpp
for (char c : message) {
    long long cipher = modExp((long long)c, e, n);
    ciphertexts.push_back(cipher);
}
```

**Vulnerability:**

- "HELLO" → `[H, E, L, L, O]` - Two L's produce identical ciphertext
- Reveals message length
- Vulnerable to frequency analysis
- Pattern matching attacks possible

**Solutions:**

#### Option A: Padding (Simpler)

Add random padding to hide patterns:

```cpp
vector<long long> encryptString(string message, long long e, long long n) {
    vector<long long> ciphertexts;

    for (char c : message) {
        // Add random salt to each character
        long long paddedChar = (long long)c * 256 + (rand() % 256);
        long long cipher = modExp(paddedChar, e, n);
        ciphertexts.push_back(cipher);
    }
    return ciphertexts;
}
```

**Pros:** Simple, each encryption unique
**Cons:** Increases ciphertext size

#### Option B: Block Cipher Mode (Better)

Pack multiple characters into blocks:

```cpp
vector<long long> encryptString(string message, long long e, long long n) {
    vector<long long> ciphertexts;

    // Calculate max chars per block
    int blockSize = 2;  // Start with 2 chars per block

    for (size_t i = 0; i < message.length(); i += blockSize) {
        long long block = 0;

        // Pack characters into one number
        for (int j = 0; j < blockSize && (i + j) < message.length(); j++) {
            block = block * 256 + (long long)message[i + j];
        }

        // Ensure block < n
        if (block >= n) {
            // Block too large, split it
            // Handle this edge case
        }

        long long cipher = modExp(block, e, n);
        ciphertexts.push_back(cipher);
    }
    return ciphertexts;
}
```

**Pros:** Fewer ciphertext numbers, more secure
**Cons:** More complex, must handle block boundaries

**Priority:** High (demonstrates real-world RSA better)

---

## Implementation Plan

### Phase 1: Random Key Generation

1. Add `#include <random>` and `#include <ctime>` to `main.cpp`
2. Modify `genKeys()` function with randomization logic
3. Test: Run program multiple times, verify different keys

### Phase 2: Block Cipher Mode

1. Create new encryption function: `encryptStringBlocks()`
2. Create corresponding decryption: `decryptStringBlocks()`
3. Add menu option to choose encryption mode
4. Compare outputs: character-by-character vs blocks

### Phase 3: Visual Comparison Tool

Add option 6: "Compare Encryption Methods"

- Show character-by-character ciphertext
- Show block-mode ciphertext
- Demonstrate security improvement

---

## Educational Value

**Keep character-by-character mode** to demonstrate:

- Why naive RSA is vulnerable
- Importance of padding/blocking
- Real-world security considerations

**Add block mode** to show:

- How production RSA works
- Security improvements
- Trade-offs (complexity vs security)

---

## Testing Checklist

- [ ] Random keys generate different values each run
- [ ] Block cipher mode encrypts successfully
- [ ] Block cipher mode decrypts to original message
- [ ] Same message produces different ciphertexts with padding
- [ ] Compare security: same input, different outputs

---

## References

- OAEP Padding: Used in real RSA implementations
- PKCS#1: RSA encryption standard
- Block cipher modes: ECB, CBC (concepts applicable to RSA)

---

**Note:** This is an educational implementation. Production RSA uses standardized libraries (OpenSSL, etc.) with proven padding schemes.
