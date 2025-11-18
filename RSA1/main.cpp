#include <iostream>
#include "json.hpp"
using json = nlohmann::json;
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

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // throw away leftover stuff INCLUDING the '\n'
    cin.get(); // now actually wait for the user to press Enter
}

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

public:

    RSA() : hasKey(false) {}

    void generate(){
        cout << "\nGenerating RSA keys..." << endl<< endl;
        if (hasKey){
            cout << "Keys already generated. Continue? (y/n)" << endl;
            char choice;
            cin >> choice;
            if (choice == 'n'){
                return;
            } 
        }
        system("cls");
        keys = genKeys();
        hasKey = true;
        cout << "\nKeys generated successfully!" << endl;
        
        cout << "\nMod: "<< keys.modulus << endl;
        cout << "\nPublic Key: " << keys.pubExp << endl;
        cout << "Private Key: " << keys.privExp << endl;
        cout << "Primes: P = " << keys.primeP << ", Q = " << keys.primeQ << endl;
    }

    // Core encryption/decryption methods (return values)
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
    
    // Convenience methods for menu (handle I/O and storage)
    void encryptStored(){
        if (!hasKey) {
            cout<<"Generate Keys First" <<endl;
            return;
        }

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
        if (!hasKey) {
            cout<<"Generate Keys First" <<endl;
            return;
        }

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
        if (!hasKey) {
            cout << "Generate Keys First" << endl;
            return;
        }
        
        cout << "\n=== Current RSA Keys ===" << endl;
        cout << "Prime P: " << keys.primeP << ", Prime Q: " << keys.primeQ << endl;
        cout << "Modulus: : " << keys.modulus << endl;
        cout << "φ(n): " << (keys.primeP - 1) * (keys.primeQ - 1) << endl;
        cout << "\nPublic Key: " << keys.pubExp << endl;
        cout << "Private Key: " << keys.privExp << endl;
    }

    void displayMessageCipher(){
        if (!hasKey) {
            cout << "Generate Keys First" << endl;
            return;
        }

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
        ifstream file("keys.json");
        if (!file.is_open()){
            cout << "Error opening keys.json" << endl;
            return;
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        json data = json::parse(content);
        file.close();
        for (auto& key : data["keys"]) {
            cout << "Name: " << key["name"] << endl;
            cout << "Modulus: " << key["modulus"] << endl;
            cout << "Public Key: " << key["public"] << endl;
            cout << "Private Key: " << key["private"] << endl;
            cout << "Prime P: " << key["primeP"] << endl;
            cout << "Prime Q: " << key["primeQ"] << endl << endl;
        }
        file.close();
    }

    void importKeys(){
        ifstream file("keys.json");
        if (!file.is_open()){
            cout << "Error opening keys.json, type keys manually: " << endl;
            cout << "Enter modulus, public, private keys:" << endl;
            cin>>keys.modulus>>keys.pubExp>>keys.privExp;
            return;
        }
        
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        
        // Parse JSON
        json data = json::parse(content);
        
        // Get key name from user
        cout << "Enter key name: ";
        string name;
        cin >> name;
        
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
                file.close();
                return;
            }
        }
        file.close();
        cout << "Key not found" << endl;
    }

    void exportKeys(){
        if (!hasKey) {
            cout << "Generate Keys First" << endl;
            return;
        }
        string name;
        cout << "Enter key name: ";
        cin >> name;

        json data;
        ifstream readFile("keys.json");

        if (readFile.is_open()){
            string content((istreambuf_iterator<char>(readFile)), istreambuf_iterator<char>());
            data = json::parse(content);
            readFile.close();
        } else {
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
        ifstream file("keys.json");
        if (!file.is_open()){
            cout << "Error opening keys.json" << endl;
            return;
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        json data = json::parse(content);
        file.close();

        cout << "Enter key name: ";
        string name;
        cin >> name;

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
