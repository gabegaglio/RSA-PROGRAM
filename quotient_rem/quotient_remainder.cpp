#include<iostream>

using namespace std;


void div (float a, float b) {

    float Q = 0, R = a;

    while (R >= b){
        R -= b;
        Q++;
    }
    cout<<endl<<endl<<"quotient = "<<Q<<" | remainder = "<<R<<endl<<endl;
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