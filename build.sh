#!/bin/bash

# RSA3 System Build Script
echo "Cleaning old build files..."
rm -f *.o main

echo "Building RSA3 Cryptography System..."

g++ -std=c++17 -DUNIFIED_BUILD \
    main.cpp \
    euclidean/euclidean.cpp \
    sieve/sieve.cpp \
    uft/uft.cpp \
    mod_exponentiation/modexp.cpp \
    -o main 2>&1

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo ""
    ./main
else
    echo "✗ Build failed!"
    exit 1
fi

