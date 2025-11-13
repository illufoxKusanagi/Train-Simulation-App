#!/bin/bash
set -e

echo "🚀 Starting Train Simulation Monorepo..."

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export NEXT_PUBLIC_API_URL="http://localhost:8080"

# Cleanup function
cleanup() {
    echo "🧹 Stopping all services..."
    pkill -P $$ || true
    exit 0
}

trap cleanup EXIT INT TERM

# Start backend
echo "🔧 Starting Qt Backend..."
cd "$PROJECT_ROOT/backend"
chmod +x scripts/smart-run.sh
./scripts/smart-run.sh &
BACKEND_PID=$!

# Wait for backend
wait_for_backend() {
    for i in {1..30}; do
        if curl -s http://localhost:8080/status > /dev/null 2>&1; then
            echo "✅ Backend ready"
            return 0
        fi
        sleep 1
    done
    return 1
}

wait_for_backend || { echo "❌ Backend failed"; exit 1; }

# Start frontend
echo "💻 Starting Next.js Frontend..."
cd "$PROJECT_ROOT/frontend"
npm run dev &
FRONTEND_PID=$!

# Wait for frontend
for i in {1..15}; do
    if curl -s http://localhost:3000 > /dev/null 2>&1; then
        echo "✅ Frontend ready at http://localhost:3000"
        break
    fi
    sleep 2
done

echo "🎉 Development environment ready!"
echo "   Frontend: http://localhost:3000"
echo "   Backend:  http://localhost:8080"
echo ""
echo "Press Ctrl+C to stop all services"

# Keep running
wait