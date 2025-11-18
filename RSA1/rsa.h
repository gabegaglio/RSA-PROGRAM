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
        int maxPrime = 10000;

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
        return json::parse(content);
    }

    // ENCRYPTION & DECRYPTION FUNCTIONS
    vector<long long> encryptString(const string& message, long long ex, long long modu) {
        vector<long long> cipherText;

        for (char c : message){
            long long num = (long long)(c);
            long long cipher = modExp(num, ex, modu);

            cipherText.push_back(cipher);
        }

        return cipherText;
    }


    string decryptString(const vector<long long>& ciphertext, long long d, long long n) {
        string result = "";

        for (long long cipher : ciphertext) {
            long long num = modExp(cipher, d, n);  
            result += (char)num;  
        }

        return result;
    }

    // wrapper functions, calls encryptString and decryptString
    vector<long long> encrypt(const string& message){
        if (!hasKey){
            cout<<"Generate Keys First" <<endl;
            return {};
        }
        return encryptString(message, keys.pubExp, keys.modulus);
    }

    string decrypt(const vector<long long>& ciphertext){
        if (!hasKey) {
            cout<<"Generate Keys First" <<endl;
            return "";
        }
        return decryptString(ciphertext, keys.privExp, keys.modulus);
    }

public:

    RSA() : hasKey(false) {}

    void generate(){
        cout << "\nGenerating RSA keys..." << endl<< endl;
        if (hasKey){
            cout << "Key already exists. Continue? (y/n)" << endl;
            char choice;
            cin >> choice;
            if (choice == 'n'){
                return;
            } 
        }
        system("clear");
        keys = genKeys();
        hasKey = true;
        cout << "\nKeys generated successfully!" << endl;
        
        cout << "\nMod: "<< keys.modulus << endl;
        cout << "\nPublic Key: " << keys.pubExp << endl;
        cout << "Private Key: " << keys.privExp << endl;
        cout << "Primes: P = " << keys.primeP << ", Q = " << keys.primeQ << endl;
    }

    // Convenience methods for menu (handle I/O and storage)
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
            cout<<"No Encrypted Message"<<endl;
            return;
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
        cout << "Modulus: : " << keys.modulus << endl;
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

    void importKeys(){
        
        json data = jsonLoadKeys();
        
        // If file doesn't exist or is empty, allow manual input
        if (data.empty() || !data.contains("keys")) {
            cout << "Error opening keys.json, type keys manually: " << endl;
            cout << "Enter modulus, public, private keys:" << endl;
            cin>>keys.modulus>>keys.pubExp>>keys.privExp;
            hasKey = true;
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
                string choice;
                cin >> choice;
                if (choice == "y"){
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
        
        ofstream file("keys.json");
        if (!file.is_open()){
            cout << "Error opening keys.json" << endl;
            return;
        }
        file << data.dump(4);
        cout << "Keys exported successfully to keys.json" << endl;
        file.close();
    }

    void deleteKeys(){
        
        json data = jsonLoadKeys();

        cout << "Enter key name: ";
        string name;
        getline(cin >> ws, name);

        bool found = false;

        // iterator based loop to correctly remove key at index
        for (auto it = data["keys"].begin(); it != data["keys"].end(); ++it) {
            if ((*it)["name"] == name){
                found = true;
                cout << "Key found, delete? (y/n)" << endl;
                string choice;
                cin >> choice;
                if (choice == "y"){
                    it = data["keys"].erase(it);
                    cout << "Key deleted successfully" << endl;
                    break;
                } else {
                    it++;
                }
            } else {
                it++;
            }
        }

        if (!found) {
            cout << "Key not found" << endl;
            return;
        }

        // write back to file
        ofstream writeFile("keys.json");
        if (!writeFile.is_open()){
            cout << "Error opening keys.json" << endl;
            return;
        }
        writeFile << data.dump(4);
        cout << "Keys deleted successfully" << endl;
        writeFile.close();
    }


};

#endif

