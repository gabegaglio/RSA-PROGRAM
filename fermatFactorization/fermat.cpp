#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
using namespace std;


vector<vector<long long>> fermatFactorization(long long n){

    vector<vector<long long>> fermats;

    if (n <= 1) cout<<n<<"not factorable"<<endl;
    long long x = ceil(sqrt(n));
    long long y;

    // step 1 
    if (n == pow(x,2)){
        cout<<x<<" is a factor of n"<<endl;
        return fermats; 
    } else {
        // push x value first
        y = sqrt(pow(x,2) - n);
        fermats.push_back({x, y});
        
        x++;
        // step 2 
        while (x <= (n+1) / 2) {
            y = sqrt(pow(x,2) - n);
            fermats.push_back({x, y});
            if (ceil(pow(y, 2) == pow(x,2) - n)){
                cout << n << " has factors "<<x<<" + "<<y<<" = " << (x + y) << " and " << x <<" - "<<y<<" = "<<(x - y) << endl;
                return fermats;
            } else {
                x++; // go to step 2 
            }
        }
        cout<<n<<" is prime "<<endl;
    }
    return fermats;
}


int main(){

    long long n;
    cout<<"Enter positive odd integer: "<<endl;
    cin>>n;
    cout<<endl<<endl;

    vector<vector<long long>> ferm = fermatFactorization(n); 
    
    // Display the values in a grid format
    if (!ferm.empty()) {
        cout << "+------+------------+" << endl;
        cout << "|  x   | sqrt(x²-n) |" << endl;
        cout << "+------+------------+" << endl;
        
        for (const auto& row : ferm) {
            double sqrt_value = sqrt(pow(row[0], 2) - n);
            cout << "| " << setw(4) << row[0] << " | " 
                 << setw(10) << fixed << setprecision(2) << sqrt_value << " |" << endl;
        }
        cout << "+------+------------+" << endl;
    }

    return 0;
}

/*


*/