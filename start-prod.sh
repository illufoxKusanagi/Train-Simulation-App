#!/bin/bash
set -e

echo "🏗️  Building Train Simulation App for Production..."

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Build backend
echo "📦 Building Qt Backend..."
cd "$PROJECT_ROOT/backend"
mkdir -p build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
echo "✅ Backend built successfully"

# Build frontend
echo "📦 Building Next.js Frontend..."
cd "$PROJECT_ROOT/frontend"
npm ci
npm run build
echo "✅ Frontend built successfully"

echo "🎉 Production build complete!"
echo "   Backend binary: backend/build-release/bin/TrainSimulationApp"
echo "   Frontend static: frontend/out/"