#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <limits>
#include <random>
#include <ctime>
#include <map>
#include <string>
#include <cctype>
#include <sstream>
#include "euclidean/euclidean.h"
#include "sieve/sieve.h"
#include "uft/uft.h"
#include "mod_exponentiation/modexp.h"
#include "fermatFactorization/fermat.h"
#include "quotrem/quotrem.h"

using namespace std;

/*
MESSAGE ENCRYPTED AN IS THIS

VINCE
*/

// textbook map conversion
map<char, int> charToNum = {
    {' ', 99},
    {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
    {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20}, {'L', 21},
    {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27},
    {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33},
    {'Y', 34}, {'Z', 35}
};

// reverse map
map<int, char> numToChar = {
    {99, ' '},
    {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'},
    {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'}, {21, 'L'},
    {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'},
    {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'}, {32, 'W'}, {33, 'X'},
    {34, 'Y'}, {35, 'Z'}
};

vector<int> encode(string& message);
vector<long long> encrypt(vector<int>& blocks, long long mod, long long pub);
vector<int> decrypt(vector<long long>& encBlocks, long long mod, long long priv);

int main(){
    
    // GET PRIMES, PUBLIC, PRIVATE, MODULUS
    // Using values from keys.json
    long long p = 787;
    long long q = 1567;
    
    // generate modulus and phi
    long long n = 10541;  // Should be 1233229
    long long phi = (p - 1) * (q - 1);

    long long pub = 5; // mine is 65537
    long long priv = 317105;

    // PRINT KEYS
    cout << "\n=== RSA Keys ===" << endl;
    cout << "p: " << p << endl;
    cout << "q: " << q << endl;
    cout << "n: " << n << endl;
    cout << "phi: " << phi << endl;
    cout << "pub: " << pub << endl;
    cout << "priv: " << priv << endl << endl;

    while (true){
        cout << "1. Encrypt"<<endl;
        cout << "2. Decrypt"<<endl;
        cout << "0. Exit"<<endl;
        cout << "\nChoice: ";

        int choice;
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 1: {
                cout << "Enter message: "<<endl;
                string message;
                getline(cin>>ws, message);
                cout << endl;
                
                vector<int> translated = encode(message);
                cout<<"Translated: "<<endl;
                for (int val : translated) cout << val << " ";
                cout<<endl<<endl;
                
                vector<long long> encryptedBlocks = encrypt(translated, n, pub);
                cout << "Encrypted blocks: " << endl;
                for (long long val : encryptedBlocks) cout << val << " ";
                cout << endl << endl;
                break;
            }
            case 2: {
                cout << "Enter encrypted blocks (space-separated): "<<endl;
                string encryptedInput;
                getline(cin>>ws, encryptedInput);
                cout << endl;
                
                vector<long long> encryptedBlocksVector;
                stringstream ss(encryptedInput);
                long long num;
                while (ss >> num) encryptedBlocksVector.push_back(num);
                
                if (encryptedBlocksVector.empty()) {
                    cout << "No valid numbers entered." << endl << endl;
                    break;
                }
                
                vector<int> decryptedBlocks = decrypt(encryptedBlocksVector, n, priv);
                cout << "Decrypted blocks: ";
                for (int val : decryptedBlocks) cout << val << " ";
                cout << endl;
                
                string decryptedMessage = "";
                for (int block : decryptedBlocks) {
                    // Split block into 2-digit chunks (e.g., 3217 -> 32, 17)
                    string blockString = to_string(block);
                    
                    if (blockString.length() % 2 != 0) {
                        blockString = "0" + blockString;
                    }
                    
                    for (size_t i = 0; i < blockString.length(); i += 2) {
                        int twoDig = stoi(blockString.substr(i, 2));
                        if(numToChar.find(twoDig) != numToChar.end()) {
                            decryptedMessage += numToChar[twoDig];
                        }
                    }
                }
                cout << "Decrypted message: " << decryptedMessage << endl << endl;
                break;
            }
            case 0:
                cout << "Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice" << endl << endl;
                break;
        }
    }
    
    return 0;
}


// translation from char to number from map
vector<int> encode(string& message){

    vector<int> blocks;
    
    // Each character maps to a 2-digit number (10-35 for letters, 99 for space)
    for(size_t i = 0; i < message.length(); i++) {
        char c = toupper(message[i]);
        
        if(charToNum.find(c) != charToNum.end()) {
            int num = charToNum[c];
            blocks.push_back(num);
        }
    }
    return blocks;
}

// translation from encoded block to encrypted
vector<long long> encrypt(vector<int>& blocks, long long mod, long long pub){

    vector<long long> encrypted;
    
    for (int block : blocks) {
        // E(block) = residue of block^e mod n
        long long encBlock = modExp(block, pub, mod);
        encrypted.push_back(encBlock);
    }
    
    return encrypted;
}

vector<int> decrypt(vector<long long>& encBlocks, long long mod, long long priv){
    
    // take decrypted blocks after encoding and encrypting
    vector<int> decrypted;

    for (long long block: encBlocks){
        long long decBlock = modExp(block, priv, mod);
        // static cast to int since decypted block is small
        decrypted.push_back(static_cast<int>(decBlock));
    }

    return decrypted;
}

void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


