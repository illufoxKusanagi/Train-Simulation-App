#!/bin/bash

# Build script for production release
BUILD_DIR="build"
BUILD_TYPE="Release"

echo "🚀 Building backend in Release mode..."

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Run CMake configuration
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Build the project
cmake --build . --config $BUILD_TYPE -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Backend build successful!"
    echo "📦 Executable: $BUILD_DIR/TrainSimulation"
else
    echo "❌ Backend build failed!"
    exit 1
fi