/*
PLAN

Represnt message as integer between 0 - n-1 and break into series of blocks

Encrypt message by raising each blow to power of e mod n
    The Result C is the remainder when M(e) is divided by n

To decrypt the ciphertext: 
    raise it to another power d,
    power d, again modulon. The encryption and decryption algorithms E and D are thus:
    C = E(M) = M (mod n), for a message M.
    D(C) = C (mod n), for a ciphertext C.

Encryption key = pair of pos ints (e, n)
Decryption key = pair of pos ints (d, n)

Encyption key = public, decryption key = private

Subscripteed as nA, eA, dA

Method:
    compute n as prod of two primes p and q
    n = p * q
        N public but p and q hidden
    Pick D to be large, random int relatively prime to (p-1)*(q-1)
    Check if D satisfies
        gcd(D, (p-1)*(q-1)) = 1
    Integer e computed from p, q, d to be multiplicaitive inverse of d, mod (p-1)*(q-1)
    so e * d ≡ 1 ( mod (p-1)*(q-1) )

Underlying Math:
    M(totient(n)) ≡ 1 (mod n)
    totient(n) is totient function giving number of pos int less than n, relatively prime to n prime to n
    totient(p) = p -1
    totient(n) = tot(p) * tot(q) = (p-1) * (q-1) = n - (p + q) + 1

    D is relatively prime to tot(n), it has multiplicative inverse e, in ring modulo tot(n)
    so D * e ≡ 1 (mod tot(n))
            
*/


#include<gmp.h>
#include<iostream>
#include<../bigInt/bigint.h>
#include<../primeGen/primeGen.h>
#include<../primality/prim.h>

using namespace std;

class RSA {

private:
    struct Keys {
        mpz_t n;
        mpz_t e;
        mpz_t d;
        mpz_t p, q;
        mpz_t phi;
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
}

public:
    RSA() {

        initKeys();
        gmp_randinit_default(randState);
        gmp_randseed_ui(randState, time(NULL));
    }

    ~RSA() {
        clearKeys();
        gmp_randclear(randState);
    }

    void generateKeys(int digitSize = 100, int primalityRounds = 40) {
        PrimeGen:genPrime(keys.p, digitSize, primalityRounds, randState);

        do {
            PrimeGen::genPrime(keys.q, digitSize, primalityRounds, randState);
        } while (mpz_cmp(keys.p, keys.q) == 0);

        mpz_mul(keys.n, keys.p, keys.q);
        mpz_t p_minus_1, q_minus_1;
        mpz_init(p_minus_1);
        mpz_init(q_minus_1);
        mpz_sub_ui(p_minus_1, keys.p, 1);
        mpz_sub_ui(q_minus_1, keys.q, 1);
        mpz_mul(keys.phi, p_minus_1, q_minus_1);
        mpz_clear(p_minus_1);
    }
}