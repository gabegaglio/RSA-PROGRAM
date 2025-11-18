#!/bin/bash

# Clean build artifacts and temporary files

echo "Cleaning build artifacts..."

# Remove object files
rm -f *.o
echo "  ✓ Removed *.o files"

# Remove temp files from Code Runner
find . -name "tempCodeRunnerFile*" -delete
echo "  ✓ Removed temp files"

# Remove main executable
rm -f rsa_system
echo "  ✓ Removed rsa_system"

echo ""
echo "✓ Cleanup complete!"
echo ""
echo "Run ./build.sh to rebuild"

