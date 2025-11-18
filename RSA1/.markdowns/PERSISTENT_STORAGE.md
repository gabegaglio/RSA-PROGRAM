# Persistent Key Storage Implementation

## Overview

This document explains how the RSA key management system persists keys across program sessions using a map-to-file serialization approach.

## Architecture

The system uses a **serialization/deserialization pattern** to persist the in-memory `map<string, Keys>` data structure to disk.

```
┌─────────────────┐         ┌──────────────┐         ┌──────────────┐
│  Program Start  │ ──────> │  Read File   │ ──────> │ Rebuild Map   │
└─────────────────┘         └──────────────┘         └──────────────┘
                                                              │
                                                              ▼
┌─────────────────┐         ┌──────────────┐         ┌──────────────┐
│  Program Exit   │ <────── │  Write File  │ <────── │ Serialize Map│
└─────────────────┘         └──────────────┘         └──────────────┘
```

## Data Structure

### In-Memory Storage

```cpp
map<string, Keys> keyStorage;  // Key name -> Keys struct
string currentKeyName;          // Currently active key
```

### Keys Structure

```cpp
struct Keys {
    long long modulus;   // n
    long long pubExp;    // e (public exponent)
    long long privExp;   // d (private exponent)
    long long primeP;    // Prime p
    long long primeQ;    // Prime q
};
```

## File Format

### Storage File: `rsa_keys.json`

**Format (JSON):**

```json
{
  "current_key": "alice",
  "keys": {
    "alice": {
      "modulus": 17869,
      "pubExp": 3,
      "privExp": 685,
      "primeP": 107,
      "primeQ": 167
    },
    "bob": {
      "modulus": 865,
      "pubExp": 3,
      "privExp": 577,
      "primeP": 5,
      "primeQ": 173
    }
  }
}
```

### Example File Content

```json
{
  "current_key": "alice",
  "keys": {
    "alice": {
      "modulus": 17869,
      "pubExp": 3,
      "privExp": 685,
      "primeP": 107,
      "primeQ": 167
    },
    "bob": {
      "modulus": 865,
      "pubExp": 3,
      "privExp": 577,
      "primeP": 5,
      "primeQ": 173
    }
  }
}
```

**Explanation:**

- `current_key`: Currently active key name
- `keys`: Object containing all key pairs, indexed by name
- Each key object contains: `modulus`, `pubExp`, `privExp`, `primeP`, `primeQ`

**Advantages of JSON:**

- Human-readable and editable
- Standard format (easy to parse/debug)
- Structured data (no need for count field)
- Can be validated
- Easy to extend with new fields

## Implementation Details

### 1. Serialization (Map → JSON File)

**Function:** `saveAllKeysToFile()`

**Process:**

1. Create JSON object
2. Add `current_key` field
3. Create `keys` object
4. For each key in map, add to `keys` object
5. Write JSON to file with pretty formatting
6. Close file

**Code (using nlohmann/json library):**

```cpp
#include "json.hpp"  // nlohmann/json header-only library
using json = nlohmann::json;

bool saveAllKeysToFile(const string& filename = "rsa_keys.json") {
    json j;

    // Set current active key
    j["current_key"] = currentKeyName;

    // Build keys object
    json keysObj = json::object();
    for (const auto& pair : keyStorage) {
        json keyObj = {
            {"modulus", pair.second.modulus},
            {"pubExp", pair.second.pubExp},
            {"privExp", pair.second.privExp},
            {"primeP", pair.second.primeP},
            {"primeQ", pair.second.primeQ}
        };
        keysObj[pair.first] = keyObj;
    }
    j["keys"] = keysObj;

    // Write to file with pretty formatting
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << j.dump(4);  // 4 spaces indentation
    file.close();
    return true;
}
```

### 2. Deserialization (JSON File → Map)

**Function:** `loadAllKeysFromFile()`

**Process:**

1. Open file for reading
2. Parse JSON from file
3. Extract `current_key` field
4. Extract `keys` object
5. For each key in `keys` object:
   - Extract key name and values
   - Insert into map
6. Activate current key if it exists
7. Close file

**Code (using nlohmann/json library):**

```cpp
#include "json.hpp"  // nlohmann/json header-only library
using json = nlohmann::json;

bool loadAllKeysFromFile(const string& filename = "rsa_keys.json") {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;  // File doesn't exist yet
    }

    json j;
    try {
        file >> j;  // Parse JSON
    } catch (const json::parse_error& e) {
        cout << "Error: Invalid JSON format!" << endl;
        file.close();
        return false;
    }

    file.close();

    // Clear existing keys
    keyStorage.clear();

    // Load current key name
    if (j.contains("current_key")) {
        currentKeyName = j["current_key"].get<string>();
    }

    // Load all keys
    if (j.contains("keys") && j["keys"].is_object()) {
        for (auto& [keyName, keyObj] : j["keys"].items()) {
            Keys k;
            k.modulus = keyObj["modulus"].get<long long>();
            k.pubExp = keyObj["pubExp"].get<long long>();
            k.privExp = keyObj["privExp"].get<long long>();
            k.primeP = keyObj["primeP"].get<long long>();
            k.primeQ = keyObj["primeQ"].get<long long>();

            keyStorage[keyName] = k;
        }
    }

    // Activate the current key if it exists
    if (!currentKeyName.empty() &&
        keyStorage.find(currentKeyName) != keyStorage.end()) {
        keys = keyStorage[currentKeyName];
        hasKey = true;
    }

    return true;
}
```

## Lifecycle

### Program Startup

```cpp
RSA() : hasKey(false), currentKeyName("") {
    // Automatically load keys from file
    if (loadAllKeysFromFile()) {
        cout << "Loaded " << keyStorage.size()
             << " key(s) from previous session." << endl;
    }
}
```

### During Execution

- All operations use in-memory `keyStorage` map
- Fast O(log n) lookups
- No file I/O during normal operations

### Key Modifications

Auto-save triggered on:

- `saveKeys()` - When saving a new key
- `deleteKey()` - When deleting a key
- `switchKey()` - When switching keys

### Program Shutdown

```cpp
~RSA() {
    if (!keyStorage.empty()) {
        saveAllKeysToFile();
    }
}
```

Or in `main()`:

```cpp
case 0: {
    rsa.saveAllKeysToFile();
    cout << "Keys saved. Goodbye!" << endl;
    return 0;
}
```

## Advantages

1. **Fast Operations**: In-memory map provides O(log n) lookups
2. **Simple Implementation**: Standard serialization pattern
3. **Persistent**: Data survives program restarts
4. **Efficient**: Only saves when needed (on changes/exit)
5. **Atomic**: Complete map state saved/loaded at once

## Limitations

1. **Not Thread-Safe**: Single-threaded file access
2. **No Encryption**: Keys stored in plain text
3. **File Corruption Risk**: No checksums or validation
4. **Single File**: All keys in one file (could be large)

## Future Improvements

1. **JSON Format**: Human-readable, easier to debug
2. **Encryption**: Encrypt the storage file
3. **Checksums**: Validate file integrity
4. **Backup System**: Keep previous versions
5. **Incremental Save**: Only save changed keys
6. **Database**: Use SQLite for larger datasets

## File Location

- **Default**: `rsa_keys.json` in program directory
- **Custom**: Can specify different filename
- **Backup**: Consider keeping backup copies

## JSON Library Setup

### Using nlohmann/json (Recommended)

1. **Download the header file:**

   ```bash
   # Download single header file
   curl -o json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
   ```

2. **Place in your project:**

   - Put `json.hpp` in your project root or include directory
   - Or use: `#include "json.hpp"` if in same directory

3. **No compilation needed:**
   - Header-only library
   - Just include it in your code
   - Works with C++11 or later

### Alternative: Manual JSON Writing (No Library)

If you prefer not to use a library, you can manually write/parse JSON, but it's more error-prone and complex.

## Error Handling

- File not found: Returns false, continues with empty map
- Invalid format: Returns false, clears map
- Write failure: Returns false, keeps in-memory data
- Read failure: Returns false, starts with empty map

## Usage Example

```cpp
RSA rsa;  // Automatically loads keys from rsa_keys.json

// Generate and save
rsa.generate();  // Prompts to save with name

// Switch between keys
rsa.switchKey("alice");  // Auto-saves

// Delete key
rsa.deleteKey("bob");  // Auto-saves

// Program exits - destructor saves all keys
```

## JSON vs Text Format Comparison

| Feature         | Text Format           | JSON Format                     |
| --------------- | --------------------- | ------------------------------- |
| Human-readable  | ✅ Yes                | ✅ Yes (better)                 |
| Editable        | ⚠️ Manual parsing     | ✅ Easy to edit                 |
| Validation      | ❌ Manual             | ✅ JSON parser validates        |
| Extensible      | ⚠️ Hard to add fields | ✅ Easy to add fields           |
| Standard        | ❌ Custom format      | ✅ Industry standard            |
| Error detection | ⚠️ Manual checks      | ✅ Parser catches errors        |
| Library needed  | ❌ None               | ✅ nlohmann/json (header-only)  |
| File size       | ✅ Smaller            | ⚠️ Slightly larger (formatting) |

## Summary

The system uses a **rebuild-on-load** approach with JSON:

- **Save**: `map<string, Keys> → JSON file` (serialization)
- **Load**: `JSON file → map<string, Keys>` (deserialization)

This is a standard pattern for persisting in-memory data structures. Using JSON provides:

- Better readability and debuggability
- Standard format that's easy to work with
- Built-in validation through JSON parser
- Easy extensibility for future features

The same core concept applies - we serialize the map to disk and rebuild it on load, just using JSON as the storage format instead of plain text.
