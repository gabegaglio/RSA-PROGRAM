# RSA Algorithms Collection

A comprehensive collection of mathematical algorithms commonly used in RSA cryptography and number theory.

## Programs Included

### 1. Euclidean Algorithm (`euclidean/`)
- **File**: `euclidean.cpp`
- **Description**: Implementation of both simple and extended Euclidean algorithms
- **Features**:
  - Simple Euclidean algorithm for finding GCD
  - Extended Euclidean algorithm for finding Bézout coefficients
  - Interactive menu system
  - Step-by-step computation display
  - Table format output for extended algorithm

### 2. Fermat Factorization (`fermatFactorization/`)
- **File**: `fermat.cpp`
- **Description**: Implementation of Fermat's factorization method
- **Features**:
  - Factors numbers using Fermat's method
  - Stores intermediate calculations in vector of pairs
  - Grid display of x and √(x²-n) values
  - Verification of factorization results

### 3. Quotient and Remainder (`quotient_rem/`)
- **File**: `quotient_remainder.cpp`
- **Description**: Basic division algorithm implementation
- **Features**:
  - Computes quotient and remainder
  - Demonstrates fundamental division concepts

### 4. Unique Factorization (`unique_factorization/`)
- **File**: `uft.cpp`
- **Description**: Implementation of unique prime factorization
- **Features**:
  - Finds all prime factors of a number
  - Demonstrates the fundamental theorem of arithmetic

## Compilation Instructions

Each program can be compiled using g++ with C++17 standard:

```bash
# For Euclidean Algorithm
cd euclidean
g++ -std=c++17 euclidean.cpp -o euclidean
./euclidean

# For Fermat Factorization
cd fermatFactorization
g++ -std=c++17 fermat.cpp -o fermat
./fermat

# For Quotient/Remainder
cd quotient_rem
g++ -std=c++17 quotient_remainder.cpp -o quotient_remainder
./quotient_remainder

# For Unique Factorization
cd unique_factorization
g++ -std=c++17 uft.cpp -o uft
./uft
```

## Mathematical Background

### Euclidean Algorithm
The Euclidean algorithm is an efficient method for computing the greatest common divisor (GCD) of two integers. The extended version also finds the Bézout coefficients.

### Fermat Factorization
Fermat's factorization method attempts to factor a number n by finding integers x and y such that n = x² - y² = (x+y)(x-y).

### Applications in RSA
These algorithms are fundamental to RSA cryptography:
- **Euclidean Algorithm**: Used for finding modular multiplicative inverses
- **Factorization**: The security of RSA relies on the difficulty of factoring large numbers
- **Prime Testing**: Essential for generating RSA key pairs

## Features

- **Interactive Menus**: User-friendly interfaces for all programs
- **Step-by-step Output**: Detailed computation steps for educational purposes
- **Vector Storage**: Efficient data structures for storing intermediate results
- **Formatted Display**: Clean, readable output with tables and grids
- **Input Validation**: Robust error handling for user inputs

## Requirements

- C++ compiler with C++17 support
- Standard C++ libraries (iostream, vector, iomanip, climits, cmath)

## Author

Gabriel Gaglio

## License

This project is open source and available under the MIT License.
