#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "fermat.h"
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
        // push first pair
        long long ySquared = pow(x,2) - n;
        y = sqrt(ySquared);
        fermats.push_back({x, y});
        
        // check if first pair/iteration gives factors
        if (y * y == ySquared){
            cout << n << " has factors "<<x<<" + "<<y<<" = " << (x + y) << " and " << x <<" - "<<y<<" = "<<(x - y) << endl;
            return fermats;
        }
        
        x++;
        // step 2 
        while (x <= (n+1) / 2) {
            long long ySquared = pow(x,2) - n;
            y = sqrt(ySquared);
            fermats.push_back({x, y});
            
            // Check if y is an integer (perfect square)
            if (y * y == ySquared){
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
        int total_rows = ferm.size();
        bool show_full = true;  // Default to full table
        
        // If more than 20 rows, ask user preference
        if (total_rows > 20) {
            char choice;
            cout << "\nTable has " << total_rows << " rows. Show:" << endl;
            cout << "\n[F] Full table" << endl;
            cout << "[C] Condensed table (first 20 + last 3)" << endl;
            cout << "Enter choice (F/C): ";
            cin >> choice;
            cout << endl;
            
            show_full = (choice == 'F' || choice == 'f');
        }
        
        cout << "+------+------------+" << endl;
        cout << "|  x   | sqrt(x²-n) |" << endl;
        cout << "+------+------------+" << endl;
        
        if (show_full || total_rows <= 20) {
            // Show all rows
            for (int i = 0; i < total_rows; i++) {
                double sqrt_value = sqrt(pow(ferm[i][0], 2) - n);
                cout << "| " << setw(4) << ferm[i][0] << " | " 
                     << setw(10) << fixed << setprecision(2) << sqrt_value << " |" << endl;
            }
        } else {
            // Show condensed version
            int max_display = 20;
            
            // Display first 20 rows
            for (int i = 0; i < max_display; i++) {
                double sqrt_value = sqrt(pow(ferm[i][0], 2) - n);
                cout << "| " << setw(4) << ferm[i][0] << " | " 
                     << setw(10) << fixed << setprecision(2) << sqrt_value << " |" << endl;
            }
            
            cout << "|  ... |        ... |" << endl;
            
            // Show last 3 rows
            int start_last = max(max_display, total_rows - 3);
            for (int i = start_last; i < total_rows; i++) {
                double sqrt_value = sqrt(pow(ferm[i][0], 2) - n);
                cout << "| " << setw(4) << ferm[i][0] << " | " 
                     << setw(10) << fixed << setprecision(2) << sqrt_value << " |" << endl;
            }
        }
        
        cout << "+------+------------+" << endl;
        
        // Check the last row to see if factors were found
        if (!ferm.empty()) {
            long long last_x = ferm.back()[0];
            long long ySquared = pow(last_x, 2) - n;
            long long y = sqrt(ySquared);
            
            cout << "\nFactors:" << endl;
            if (y * y == ySquared) {
                // Factors found
                long long factor1 = last_x + y;
                long long factor2 = last_x - y;
                cout << n << " has factors: " << factor1 << " × " << factor2 <<" = "<<factor1*factor2<< endl;
                
            } else {
                cout << n << " is prime (no factors)" << endl;
            }
        }
    }

    return 0;
}

