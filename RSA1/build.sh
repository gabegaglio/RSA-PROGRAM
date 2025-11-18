#!/bin/bash

# RSA System Build Script
echo "Cleaning old build files..."
rm -f *.o rsa_system

echo "Building RSA Cryptography System..."

g++ -std=c++17 -DUNIFIED_BUILD \
    main.cpp \
    euclidean/euclidean.cpp \
    sieve/sieve.cpp \
    uft/uft.cpp \
    mod_exponentiation/modexp.cpp \
    -o rsa_system 2>&1

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo ""
    ./rsa_system
else
    echo "✗ Build failed!"
    exit 1
fi

