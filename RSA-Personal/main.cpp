#include <iostream>
#include <vector>
#include <limits>
#include <map>
#include <algorithm>
#include "rsa.h"

using namespace std;

// Character encoding maps
map<char, int> charToNumMap = {
    {' ', 99},
    {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
    {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20}, {'L', 21},
    {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27},
    {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33},
    {'Y', 34}, {'Z', 35}
};

// Reverse map
map<int, char> numToCharMap = {
    {99, ' '},
    {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'},
    {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'}, {21, 'L'},
    {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'},
    {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'}, {32, 'W'}, {33, 'X'},
    {34, 'Y'}, {35, 'Z'}
};

// declarations for helper functions
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
        cout << "10. Import Encrypted Message" << endl;
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
            case 10: {
                rsa.importCiphertext();
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
    // Convert to uppercase for case-insensitive encoding
    char upperC = toupper(c);
    
    // Check if character is in the map
    if (charToNumMap.find(upperC) != charToNumMap.end()){
        return charToNumMap[upperC];
    }
    
    // Fallback to ASCII if not in map
    cout << "Warning: Character '" << c << "' not in encoding map, using ASCII value" << endl;
    return (long long)(c);
}

vector<long long> stringToNum(string message){
    vector<long long> nums;

    for (char c : message){
        nums.push_back(charToNum(c));
    }

    return nums;
}

string numToString(vector<long long> nums){
    string result = "";

    for (long long num : nums){
        // Check if number is in the reverse map
        if (numToCharMap.find((int)num) != numToCharMap.end()){
            result += numToCharMap[(int)num];
        } else {
            // Fallback to ASCII conversion if not in map
            cout << "Warning: Number " << num << " not in decoding map, using ASCII conversion" << endl;
            result += (char)num;
        }
    }

    return result;
}
