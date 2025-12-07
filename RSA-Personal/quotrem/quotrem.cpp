#include<iostream>
#include "quotrem.h"

using namespace std;

std::pair<long long, long long> divisionAlgorithm(long long a, long long b){
    long long Q = 0, R = a;
    
    while (R >= b){
        R -= b;
        Q++;
    }
    
    return {Q, R};
}

void div (float a, float b) {
    auto [q, r] = divisionAlgorithm((long long)a, (long long)b);
    cout<<endl<<endl<<"quotient = "<<q<<" | remainder = "<<r<<endl<<endl;
}

int main(){

    while (true) {
    
    cout<<"-----------------------"<<endl<<"enter two positive numbers"<<endl;
    float first, second;
    cin>>first;
    cin>>second;

    div(first, second);
    
    }

    return 0;
}