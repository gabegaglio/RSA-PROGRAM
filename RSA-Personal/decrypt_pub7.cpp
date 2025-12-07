#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "mod_exponentiation/modexp.h"

using namespace std;

map<int, char> numToChar = {
    {99, ' '},
    {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'},
    {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'}, {21, 'L'},
    {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'},
    {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'}, {32, 'W'}, {33, 'X'},
    {34, 'Y'}, {35, 'Z'}
};

string decrypt(vector<long long>& blocks, long long e, long long n) {
    string message = "";
    
    cout << "Decrypted blocks: ";
    for (long long cipher : blocks) {
        long long plain = modExp(cipher, e, n);
        cout << plain << " ";
        
        if (numToChar.find((int)plain) != numToChar.end()) {
            message += numToChar[(int)plain];
        }
    }
    cout << endl;
    
    return message;
}

int main() {
    vector<long long> encryptedMsg = {638, 4431, 7686, 7686, 9250, 3985, 4431, 5720, 4431, 4489, 11639, 11311, 5886, 6598, 7211, 4431, 6607, 5720, 9250, 4489, 5720, 193, 7686, 5720, 7211, 11910, 193, 7686};
    
    vector<long long> encryptedSig = {6835, 193, 4489, 11639, 4431};
    
    // Public key from RSA-Class
    long long pubKey = 7;
    long long mod = 13321;
    
    cout << "=== DECRYPTING/VERIFYING WITH PUBLIC KEY ===" << endl;
    cout << "Using e=" << pubKey << ", n=" << mod << endl << endl;
    
    cout << "Message:" << endl;
    string message = decrypt(encryptedMsg, pubKey, mod);
    cout << "Decrypted: \"" << message << "\"" << endl << endl;
    
    cout << "Signature:" << endl;
    string signature = decrypt(encryptedSig, pubKey, mod);
    cout << "Decrypted: \"" << signature << "\"" << endl;
    
    return 0;
}
