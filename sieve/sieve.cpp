#include<iostream>
#include<vector>
#include<iomanip>
#include "sieve.h"

using namespace std;

vector<long long> sieve(long long n){

    vector<long long> v((n+1)/2, 1);
    vector<long long> ans;

    long long p = 3;
       
    while (p*p <= n){
            
        if (v[(p-1)/2] == 0){
            p += 2;
        } else {
            long long t = p*p;

            while (t <= n){
                v[(t-1)/2] = 0;
                t += 2*p;
            }
            p += 2;
        }        
    }

    for (int i = 1; i < v.size(); i++)
        if (v[i] == 1)
            ans.push_back(2*i + 1);  
    
    return ans;
}

#ifndef UNIFIED_BUILD
int main() {
    long long n;

    while (true) {
        cout << "Enter an odd positive integer (0 to quit): " << endl;
        cin >> n;

        if (n == 0) break; // exit condition
        if (n <= 0 || n % 2 == 0) {
            continue;
        }

        vector<long long> sie = sieve(n);

        cout << "Odd primes up to " << n << ": ";
        for (long long val : sie)
            cout << val << " ";
        cout << endl;
    }

    return 0;
}
#endif

/*
Input: an odd positive integer n
Output: the list of all odd positive primes less than or equal to n
Step 1 
    Begin by creating a vector v with (n - 1)/2 entries, 
    each of which will be initializedwith 1, 
    and letting P = 3.

Step 2 
    If p^2 >n, 
    write the list of numbers 2j + 1 for which the jth entry of the vector v is 1 
    and stop; 
    otherwise go to step 3.

Step 3 
    if the entry indexed by (P - 1)/2 of the vector v is 0, 
    increase P by 2 
    and return to step 2; 
        otherwise go to step 4.

Step 4 
    Give to a new variable T the value P^2; 
    1) replace with 0 the value of the entry indexed by (T - 1)/2 of the vector v 
    2) increase T by 2P; 
    repeat these two steps until T > n, 
    then increase P by 2 and return to step 2.
*/