#!/usr/bin/env bash
set -e
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
echo "=== Zbudowano: build/tqztr_editor ==="
