#ifndef RSA_H
#define RSA_H

#include <iostream>
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#include <limits>
#include <random>
#include <ctime>
#include <sstream>
#include "euclidean/euclidean.h"
#include "sieve/sieve.h"
#include "uft/uft.h"
#include "mod_exponentiation/modexp.h"
#include "fermatFactorization/fermat.h"
#include "quotrem/quotrem.h"

using namespace std;
using json = nlohmann::json;

class RSA {
private:
    struct Keys {
        long long modulus;        // modulus
        long long pubExp;         // public exponent
        long long privExp;        // private exponent
        long long primeP, primeQ; // primes 
    };
    
    // global variables
    Keys keys;
    bool hasKey = false;
    vector<long long> storedCipher;
    string storedMessage = "";

    Keys genKeys(){
        Keys keys;
        int maxPrime = 1000;  // to ensure phi(n) > 65537

        // use sieves to generate vector of primes
        vector<long long> primes = sieve(maxPrime);
        
        // generate two random indexes
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, primes.size() - 1);
        int index1 = dis(gen);
        int index2 = dis(gen);

        // if indexes are the same, generate new indexes
        while (index1 == index2) {
            index2 = dis(gen);
        }   

        // grab two primes from primes vector
        long long p = primes[index1];
        long long q = primes[index2];
        keys.primeP = p;
        keys.primeQ = q;

        // generate modulus and phi
        keys.modulus = p * q;

        // ensure n exceeds 10,000 (assignment requirement)
        if (keys.modulus <= 10000){
            return genKeys();
        }
        long long phi = (p - 1) * (q - 1);

        // choose public exponent e, given out
        keys.pubExp = 65537; 

        // if gcd != 1, modular inverse doesnt exist, no private key 
        if (keys.pubExp >= phi || euclidean(keys.pubExp, phi) != 1){
            keys.pubExp = 3; // restart at 3
            while (euclidean(keys.pubExp, phi) != 1)
                keys.pubExp += 2; // go up by 2 until condition is met
        }

        // find private exponent 
        keys.privExp = modularInv(keys.pubExp, phi);

        return keys;
    }

    bool checkHasKey(){
        if (!hasKey){
            cout<<"Generate or Import Keys First" <<endl;
            return false;
        }
        return true;
    }

    json jsonLoadKeys(){
        ifstream file("keys.json");
        if (!file.is_open()){
            return json();  // Return empty JSON if file doesn't exist
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
        if (content.empty()) {
            return json();  // Return empty JSON if file is empty
        }
        try {
            return json::parse(content);
        } catch (const json::parse_error& e) {
            cout << "Error parsing keys.json: " << e.what() << endl;
            return json();  // Return empty JSON on parse error
        }
    }

    // helper function to save keys, called in exportKeys and deleteKeys
    bool saveKeysToFile(const json& data){
        ofstream file("keys.json");
        if (!file.is_open()){
            cout << "Error opening keys.json" << endl;
            return false;
        }
        file << data.dump(4);
        file.close();
        return true;
    }

    // ENCRYPTION & DECRYPTION FUNCTIONS
    // Helper functions for encoding/decoding
    long long encodeChar(char c) {
        char upperC = toupper(c);
        
        // Character encoding map
        static const map<char, int> charToNumMap = {
            {' ', 99},
            {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
            {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20}, {'L', 21},
            {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27},
            {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33},
            {'Y', 34}, {'Z', 35}
        };
        
        auto it = charToNumMap.find(upperC);
        if (it != charToNumMap.end()) {
            return it->second;
        }
        
        // Fallback to ASCII if not in map
        cout << "Warning: Character '" << c << "' not in encoding map, using ASCII value" << endl;
        return (long long)(c);
    }
    
    char decodeNum(long long num) {
        // Reverse character map
        static const map<int, char> numToCharMap = {
            {99, ' '},
            {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'},
            {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'}, {21, 'L'},
            {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'},
            {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'}, {32, 'W'}, {33, 'X'},
            {34, 'Y'}, {35, 'Z'}
        };
        
        auto it = numToCharMap.find((int)num);
        if (it != numToCharMap.end()) {
            return it->second;
        }
        
        // Fallback to ASCII conversion if not in map
        cout << "Warning: Number " << num << " not in decoding map, using ASCII conversion" << endl;
        return (char)num;
    }

    vector<long long> encryptString(const string& message, long long ex, long long modu) {
        vector<long long> cipherText;

        for (char c : message){
            // encode
            long long num = encodeChar(c);
            if (num >= modu) {
                cout << "Warning: Encoded value " << num << " ('" << c << "') exceeds modulus " << modu << endl;
                cout << "Encryption may fail. Consider using larger keys." << endl;
            }
            long long cipher = modExp(num, ex, modu);
            cipherText.push_back(cipher);
        }

        return cipherText;
    }

    string decryptString(const vector<long long>& ciphertext, long long d, long long n) {
        string result = "";

        for (long long cipher : ciphertext) {
            if (cipher >= n) {
                cout << "Error: Ciphertext value " << cipher << " exceeds modulus " << n << endl;
                return "";  // Invalid ciphertext
            }
            long long num = modExp(cipher, d, n);
            // Use decoding function to convert number back to char
            result += decodeNum(num);
        }

        return result;
    }

    // wrapper functions, calls encryptString and decryptString
    vector<long long> encrypt(const string& message){
        if (!hasKey){
            cout<<"Generate or Import Keys First" <<endl;
            return {};
        }
        return encryptString(message, keys.pubExp, keys.modulus);
    }

    string decrypt(const vector<long long>& ciphertext){
        if (!hasKey) {
            cout<<"Generate or Import Keys First" <<endl;
            return "";
        }
        return decryptString(ciphertext, keys.privExp, keys.modulus);
    }

public:

    RSA() : hasKey(false) {}

    void generate(){
        cout << "\nGenerating RSA keys..." << endl << endl;
        if (hasKey){
            cout << "Key already exists. Continue? (y/n)" << endl;
            char continueChoice;
            cin >> continueChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer
            if (continueChoice == 'n'){
                return;
            } 
        }
        // Cross-platform screen clear
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        keys = genKeys();
        hasKey = true;
        cout << "\nKeys generated successfully!" << endl;
        
        cout << "\nMod: " << keys.modulus << endl;
        cout << "Public Key: " << keys.pubExp << endl;
        cout << "Private Key: " << keys.privExp << endl;
        cout << "Primes: P = " << keys.primeP << ", Q = " << keys.primeQ << endl;
    }

    // convenience methods for menu (handle I/O and storage)
    // calls encrypt/decrypt which calls encryptString/decryptString
    void encryptStored(){

        if (!checkHasKey()) return;

        string message;
        cout<<"Enter message: ";
        getline(cin >> ws, message);
        storedMessage = message;
        storedCipher = encrypt(message);

        cout << "\nCipher Text: ";
        for (long long c : storedCipher) {
            cout << c << " ";
        }
        cout << endl;
    }

    void decryptStored(){
        
        if (!checkHasKey()) return;

        if (storedCipher.empty()){
            cout << "No encrypted message stored." << endl;
            cout << "Would you like to import encrypted blocks? (y/n): ";
            char response;
            cin >> response;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (response == 'y' || response == 'Y') {
                importCiphertext();
                if (storedCipher.empty()) {
                    return;  
                }
            } else {
                return;
            }
        }

        cout << "\nEncrypted: ";
        for (long long c : storedCipher) {
            cout << c << " ";
        }
        cout << endl;

        string decrypted = decrypt(storedCipher);
        cout << "\nDecrypted: " << decrypted << endl;
    }

    void displayKeys(){
        
        if (!checkHasKey()) return;
        
        cout << "\n=== Current RSA Keys ===" << endl;
        cout << "Prime P: " << keys.primeP << ", Prime Q: " << keys.primeQ << endl;
        cout << "Modulus: " << keys.modulus << endl;
        cout << "φ(n): " << (keys.primeP - 1) * (keys.primeQ - 1) << endl;
        cout << "\nPublic Key: " << keys.pubExp << endl;
        cout << "Private Key: " << keys.privExp << endl;
    }

    void displayMessageCipher(){
        
        if (!checkHasKey()) return;

        if (storedMessage.empty()) {
            cout << "No message encrypted yet!" << endl;
            return;
        }

        cout << "\n=== Message & Ciphertext ===" << endl;
        cout << "Original Message: " << "'"<<storedMessage<<"'"<< endl;
        cout << "Ciphertext: ";
        for (long long c : storedCipher) {
            cout << c << " ";
        }
        cout << endl;
    }

    void listKeys(){
        
        json data = jsonLoadKeys();
        
        if (data.empty() || !data.contains("keys") || data["keys"].empty()) {
            cout << "No keys found in keys.json" << endl;
            return;
        }

        for (auto& key : data["keys"]) {
            cout << "Name: " << key["name"] << endl;
            cout << "Modulus: " << key["modulus"] << endl;
            cout << "Public Key: " << key["public"] << endl;
            cout << "Private Key: " << key["private"] << endl;
            cout << "Prime P: " << key["primeP"] << endl;
            cout << "Prime Q: " << key["primeQ"] << endl << endl;
        }
    }

    void importMessage(){

        if (!checkHasKey()) return;

        string message;
        cout<<"Enter message: ";
        getline(cin >> ws, message);
        storedMessage = message;
        storedCipher = encrypt(message);
        cout << "\nCipher Text: ";
        for (long long c : storedCipher) {
            cout << c << " ";
        }
        cout << endl;

    }

    void importCiphertext(){
        
        if (!checkHasKey()) return;

        cout << "Enter encrypted blocks (space-separated numbers): ";
        string input;
        getline(cin >> ws, input);
        
        // Clear previous ciphertext
        storedCipher.clear();
        storedMessage = "";  // Clear stored message since we're importing cipher
        
        // Parse space-separated numbers
        stringstream ss(input);
        long long num;
        while (ss >> num) {
            storedCipher.push_back(num);
        }
        
        if (storedCipher.empty()) {
            cout << "No valid numbers entered." << endl;
            return;
        }
        
        cout << "\nImported " << storedCipher.size() << " encrypted blocks." << endl;
        cout << "Use option 3 to decrypt the message." << endl;
    }

    void importKeys(){
        cout << "Import Method: " << endl;
        cout << "1. JSON Import" << endl;
        cout << "2. Manual Input" << endl;
        cout << "Enter choice: ";
        int choice;
        cin >> choice;
        cout << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer
        
        if (choice == 2){
            cout << "Enter modulus, public, private keys:" << endl;
            cin>>keys.modulus>>keys.pubExp>>keys.privExp;
            keys.primeP = 0;
            keys.primeQ = 0;
            hasKey = true;
            exportKeys();
            return;
        }

        // Choice 1: JSON Import
        json data = jsonLoadKeys();
        
        if (data.empty() || !data.contains("keys") || data["keys"].empty()) {
            cout << "No keys found in keys.json" << endl;
            cout << "Use option 2 (Manual Input) to create a new key." << endl;
            return;
        }
        
        // Get key name from user
        cout << "Enter key name: ";
        string name;
        getline(cin >> ws, name);

        // Loop through the "keys" array
        for (auto& key : data["keys"]){
            if (key["name"] == name){
                keys.modulus = key["modulus"];
                keys.pubExp = key["public"];
                keys.privExp = key["private"];
                keys.primeP = key["primeP"];
                keys.primeQ = key["primeQ"];
                hasKey = true;
                cout << "Keys imported successfully!" << endl << endl;
                cout << "Modulus: " << keys.modulus << endl;
                cout << "Public Key: " << keys.pubExp << endl;
                cout << "Private Key: " << keys.privExp << endl;
                cout << "Prime P: " << keys.primeP << endl;
                cout << "Prime Q: " << keys.primeQ << endl;
                return;
            }
        }
        cout << "Key not found" << endl;
    }

    void exportKeys(){
        if (!checkHasKey()) return;

        string name;
        cout << "Enter key name: ";
        getline(cin >> ws, name);

        json data = jsonLoadKeys();
        
        // Initialize empty array if file doesn't exist
        if (data.empty() || !data.contains("keys")) {
            data["keys"] = json::array();
        }
        
        json newKey = {
            {"name", name},
            {"modulus", keys.modulus},
            {"public", keys.pubExp},
            {"private", keys.privExp},
            {"primeP", keys.primeP},
            {"primeQ", keys.primeQ}
        };

        bool found = false;
        for (auto& key : data["keys"]){
            if (key["name"] == name){
                found = true;
                cout << "Key already exists, update? (y/n)" << endl;
                string updateChoice;
                cin >> updateChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer
                if (updateChoice == "y"){
                    key = newKey;
                    cout << "Key updated successfully!" << endl;
                    break;
                } else {
                    cout << "Key not updated" << endl;
                    return;
                }
            }
        }
        if (!found){
            data["keys"].push_back(newKey);
        }
        
        if (saveKeysToFile(data)) {
            cout << "Keys exported successfully to keys.json" << endl;
        }
    }

    void deleteKeys(){
        cout << "Enter key name: ";
        string name;
        getline(cin >> ws, name);

        json data = jsonLoadKeys();

        if (data.empty() || !data.contains("keys") || data["keys"].empty()) {
            cout << "No keys found in keys.json" << endl;
            return;
        }

        bool found = false;

        // iterator based loop to correctly remove key at index
        for (auto it = data["keys"].begin(); it != data["keys"].end(); ) {
            if ((*it)["name"] == name){
                found = true;
                cout << "Key found, delete? (y/n)" << endl;
                string deleteChoice;
                cin >> deleteChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear input buffer
                if (deleteChoice == "y"){
                    it = data["keys"].erase(it);
                    break;
                } else {
                    it++;  // User said no, continue to next
                }
            } else {
                it++;  // Not the key we're looking for, continue
            }
        }

        if (!found) {
            cout << "Key not found" << endl;
            return;
        }

        // write back to file
        if (saveKeysToFile(data)) {
            cout << "Key deleted successfully" << endl;
        }
    }
};

#endif

