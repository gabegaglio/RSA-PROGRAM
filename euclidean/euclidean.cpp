#include<iostream>
#include<vector>
#include<iomanip>
#include<climits>

using namespace std;

long long euclidean(long long a, long long b){                           // was float; made long long
    long long A = a, B = b;
    while (true){
        long long R = A % B;
        if (R == 0) return B;                         // gcd is current B
        A = B;
        B = R;
    }
}

vector<vector<long long>> extendedEuclidean(long long a, long long b){
    vector<vector<long long>> v; // could also use pair

    // Initial rows: {remainder, quotient, x, y}
    v.push_back({a, 0, 1, 0});  // First row: a, *, 1, 0
    v.push_back({b, 0, 0, 1});  // Second row: b, *, 0, 1

    long long A = a, B = b;
    long long R = b; 

    while (R != 0){
        R = A % B;
        cout<<"remainder "<<R<<endl;
        
        long long quotient = A / B;
        cout<<"quotient "<<quotient<<endl;

        if (R == 0) {
            // push final row
            v.push_back({R, quotient, LLONG_MIN, LLONG_MIN}); // llong min for null
            break;
        }

        long long x = v[(int)v.size() - 2][2] - quotient * v[(int)v.size() - 1][2];  // alpha
        long long y = v[(int)v.size()- 2 ][3] - quotient * v[(int)v.size() - 1][3];  // beta

        // Push back quadruple: {remainder, quotient, x, y}
        v.push_back({R, quotient, x, y});

        A = B;
        cout<<"A = "<<A<<endl;
        B = R;
        cout<<"B = "<<B<<endl<<endl;
    }

    return v;
}



/*
Step 1 Begin by setting A= a and R= B= b.
Step 2 Replace the value of R by the remainder of the division of A by B, and go to step 3.
Step 3 If R= 0, write "the greatest common divisor of a and b is B" and stop; otherwise, go to step 4.
Step 4 Replace the value of A with that of B, and the value of B with that of R; return to step 2.
*/

void displayMenu() {
    cout << "\n=== Euclidean Algorithm Menu ===" << endl;
    cout << "1. Simple Euclidean Algorithm (GCD only)" << endl;
    cout << "2. Extended Euclidean Algorithm (GCD + Bezout coefficients)" << endl;
    cout << "3. Exit" << endl;
    cout << "Choose an option (1-3): ";
}

void runSimpleEuclidean() {
    long long a, b;
    cout << "\n--- Simple Euclidean Algorithm ---" << endl;
    cout << "Enter two numbers:\n";
    cin >> a >> b;
    cout << endl;
    
    long long gcd = euclidean(a, b);
    cout << "GCD(" << a << ", " << b << ") = " << gcd << endl;
}

void runExtendedEuclidean() {
    long long a, b;
    vector<vector<long long>> extended;
    
    cout << "\n--- Extended Euclidean Algorithm ---" << endl;
    cout << "Enter two numbers:\n";
    cin >> a >> b;
    cout << endl;

    cout << "Computing Extended Euclidean..." << endl;
    extended = extendedEuclidean(a, b);

    cout << endl;
    cout << "remainders | quotients |     x     |     y\n";
    cout << "-------------------------------------------\n";

    for (int i = 0; i < (int)extended.size(); i++) {
        cout << setw(10) << extended[i][0] << " | ";  // remainder
        if (i < 2) {
            cout << setw(9) << "*" << " | ";          // quotient (first two rows)
        } else {
            cout << setw(9) << extended[i][1] << " | "; // quotient
        }
        
        // Check if x and y are null (represented by LLONG_MIN)
        if (extended[i][2] == LLONG_MIN) {
            cout << setw(9) << "*" << " | ";          // x is null
        } else {
            cout << setw(9) << extended[i][2] << " | "; // x
        }
        
        if (extended[i][3] == LLONG_MIN) {
            cout << setw(9) << "*" << "\n";           // y is null
        } else {
            cout << setw(9) << extended[i][3] << "\n"; // y
        }
    }

    cout << "\nAlpha and Beta (Bezout coefficients):" << endl;
    
    int k = (int)extended.size() - 2;              // second-to-last row (last row with valid x,y)
    cout << "Alpha: " << extended[k][2] << endl;
    cout << "Beta: "  << extended[k][3] << endl;
    cout << "Verification: " << extended[k][2] << " * " << a << " + " << extended[k][3] << " * " << b << " = " << (extended[k][2] * a + extended[k][3] * b) << endl;
}

int main(){
    int choice;
    
    cout << "Euclidean Algorithm" << endl;
    
    while (true) {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                runSimpleEuclidean();
                break;
            case 2:
                runExtendedEuclidean();
                break;
            case 3:
                cout << "Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
                break;
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    return 0;
}