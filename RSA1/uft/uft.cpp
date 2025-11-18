#include <iostream>
#include <cmath>
#include "uft.h"
using namespace std;

void uft (int n){
    int f = 2;

    while (f <= sqrt(n)){
        if ((n % f) == 0){
            cout<<f<<" is a factor of "<<n<<endl<<endl;
            return;
        } f++;
    }

    cout<< n <<" is prime"<<endl<<endl;
}

bool isPrime(long long n){
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    long long f = 3;
    while (f <= sqrt(n)){
        if ((n % f) == 0) return false;
        f += 2;
    }
    return true;
}

long long smallestFactor(long long n){
    if (n <= 1) return -1;
    if (n % 2 == 0) return 2;
    
    long long f = 3;
    while (f <= sqrt(n)){
        if ((n % f) == 0) return f;
        f += 2;
    }
    return -1;  // n is prime
}

#ifndef UNIFIED_BUILD
int main() {
    
    while (true){
    int a;
    
    cout<<"Enter integer: "<<endl;
    cin>>a;

    uft(a);
    }
    return 0;
}
#endif


/*
Input: 
    positive integer n

Output: 
    positive integer f > 1, which is the smallest prime factor of n
    or a message stating that n is prime

Step 1 
    Begin by setting F= 2.

Step 2: 
    if n / f is an integer, 
    write "F si afactor of n" 
    and stop; 
    otherwise go to step 3.

Step 3 
    Increase F by one, 
    and go to step 4.

Step 4 
    If F ≥ [sqrt(n) write "n is prime" 
    and stop; 
    otherwise return to step 2.
*/
