#include <iostream>
#include "modexp.h"
using namespace std;

long long modExp(long long base, long long exp, long long mod){

    long long result = 1;

    base %= mod;

    // step 2
    while (exp > 0){
        // step 3
        if (exp & 1) result = (result * base) % mod; // step 4
        //step 5
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

#ifndef UNIFIED_BUILD
int main(){

    while (true){
        long long base, exp, mod;
        
        cout << "\n=== Modular Exponentiation ===" << endl;
        cout << "Compute: (base^exp) mod m" << endl;
        cout << "\nEnter base (0 to quit): ";

        cin >> base;
        if (base == 0) break;

        cout<< "Enter Exponent: ";
        cin>>exp;

        cout << "Enter Modulus: ";
        cin >> mod;

        long long result = modExp(base, exp, mod);
        cout << "\n(" << base << "^" << exp << ") mod " << mod << " = " << result << endl;
    }

    return 0;
}
#endif