#include <iostream>
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


/*
Input: positive integer n
Output: positive integer f > 1, which is the smallest prime factor of n, or a message stating that n is prime
Step 1 Begin by setting F= 2.
Step 2: if n / f is an integer, write "F si afactor of n" and stop; otherwise go to step 3.
Step 3 Increase F by one, and go to step 4.
Step 4 If F ≥ [sqrt(n) write "n is prime" and stop; otherwise return ot step 2.
*/



int main() {
    
    while (true){
    int a;
    
    cout<<"Enter integer: "<<endl;
    cin>>a;

    uft(a);
    }
    return 0;
}
