#include <iostream>
#include <vector>
#include <limits>
#include "rsa.h"

using namespace std;

void waitForEnter();
long long charToNum(char c);
vector<long long> stringToNum(string message);
string numToString(vector<long long> nums);

int main(){
    RSA rsa;
    
    while (true) {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║     RSA Cryptography System            ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "1. Generate RSA Keys" << endl;
        cout << "2. Encrypt Message" << endl;
        cout << "3. Decrypt Message" << endl;
        cout << "4. Display Current Key" << endl;
        cout << "5. Display Message & Cipher" <<endl;
        cout << "6. Import Key" << endl;
        cout << "7. Export Key " << endl;
        cout << "8. Delete Key" << endl;
        cout << "9. List All Keys" << endl;
        cout << "0. Exit" << endl;
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        cout << endl;
        
        switch(choice) {
            case 1: {  // Generate Keys
                rsa.generate();
                break;
            }
            case 2: { // Encrypt message
                rsa.encryptStored();
                break;
            }
            case 3: { // Decrypt message
                rsa.decryptStored();
                break;
            }
            case 4: {  // Display Keys
                rsa.displayKeys();
                break;
            }

            case 5: { // display current message + ciphertext
                rsa.displayMessageCipher();

                break;
            }

            case 6: {
                rsa.importKeys();
                break;
            }
            
            case 7: {
                rsa.exportKeys();
                break;
            }

            case 8: {
                rsa.deleteKeys();
                break;
            }
            case 9: {
                rsa.listKeys();
                break;
            }
            case 0:
                cout << "Goodbye!" << endl;
                return 0;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        
        waitForEnter();
    }  
    
    return 0;
}


void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // throw away leftover stuff INCLUDING the '\n'
    cin.get(); // now actually wait for the user to press Enter
}

long long charToNum(char c){
    return (long long)(c);
}

vector<long long> stringToNum(string message){

    vector<long long> nums;

    for (char c : message){
        nums.push_back((long long)(c));
    }

    return nums;
}

string numToString(vector<long long> nums){

    string result = "";

    for (long long num: nums){
        result += char(num);
    }

    return result;
}
