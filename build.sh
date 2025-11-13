#!/bin/bash
# 🚀 Train App - Main Build & Run Script
# Usage: ./build.sh [dev|prod|clean|install|run]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Project root directory (where this script is located)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$PROJECT_ROOT/backend"
FRONTEND_DIR="$PROJECT_ROOT/frontend"

# Build directories
BACKEND_BUILD_DIR="$BACKEND_DIR/build"
FRONTEND_BUILD_DIR="$FRONTEND_DIR/.next"

# PID tracking
FRONTEND_PID=""
BACKEND_PID=""

# Print colored message
print_msg() {
    local color=$1
    local msg=$2
    echo -e "${color}${msg}${NC}"
}

# Print section header
print_header() {
    echo ""
    print_msg "$BLUE" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$BLUE" "  $1"
    print_msg "$BLUE" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

# Check if command exists
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_msg "$RED" "❌ Error: $1 is not installed"
        exit 1
    fi
}

# Check prerequisites
check_prerequisites() {
    print_header "Checking Prerequisites"
    
    print_msg "$YELLOW" "Checking required tools..."
    check_command cmake
    check_command node
    check_command npm
    
    print_msg "$GREEN" "✅ All prerequisites satisfied"
}

# Clean build artifacts
clean_build() {
    print_header "Cleaning Build Artifacts"
    
    # Clean backend
    if [ -d "$BACKEND_BUILD_DIR" ]; then
        print_msg "$YELLOW" "🧹 Cleaning backend build..."
        rm -rf "$BACKEND_BUILD_DIR"
    fi
    
    # Clean frontend
    if [ -d "$FRONTEND_BUILD_DIR" ]; then
        print_msg "$YELLOW" "🧹 Cleaning frontend build..."
        cd "$FRONTEND_DIR"
        rm -rf .next out node_modules/.cache
    fi
    
    print_msg "$GREEN" "✅ Clean complete"
}

# Install all dependencies
install_dependencies() {
    print_header "Installing Dependencies"
    
    # Install frontend dependencies
    print_msg "$YELLOW" "📦 Installing frontend dependencies..."
    cd "$FRONTEND_DIR"
    npm install
    
    print_msg "$GREEN" "✅ All dependencies installed"
    cd "$PROJECT_ROOT"
}

# Build frontend (Next.js)
build_frontend() {
    local mode=$1
    print_header "Building Frontend (Next.js)"
    
    cd "$FRONTEND_DIR"
    
    # Install dependencies if needed
    if [ ! -d "node_modules" ]; then
        print_msg "$YELLOW" "📦 Installing npm dependencies..."
        npm install
    fi
    
    if [ "$mode" = "dev" ]; then
        print_msg "$YELLOW" "🔨 Building frontend for development..."
        npm run build
    else
        print_msg "$YELLOW" "🔨 Building frontend for production..."
        npm run build
        
        # Export static files for Qt WebEngine
        print_msg "$YELLOW" "📦 Exporting static files..."
        npm run export 2>/dev/null || print_msg "$YELLOW" "⚠️  Using .next build (add 'export' script for static export)"
    fi
    
    print_msg "$GREEN" "✅ Frontend build complete"
    cd "$PROJECT_ROOT"
}

# Build backend (Qt)
build_backend() {
    local mode=$1
    print_header "Building Backend (Qt + WebEngine)"
    
    cd "$BACKEND_DIR"
    
    # Create build directory
    mkdir -p "$BACKEND_BUILD_DIR"
    cd "$BACKEND_BUILD_DIR"
    
    if [ "$mode" = "dev" ]; then
        print_msg "$YELLOW" "🔨 Configuring CMake for development..."
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        
        print_msg "$YELLOW" "🔨 Building Qt application (Debug)..."
        cmake --build . --config Debug
    else
        print_msg "$YELLOW" "🔨 Configuring CMake for production..."
        cmake -DCMAKE_BUILD_TYPE=Release ..
        
        print_msg "$YELLOW" "🔨 Building Qt application (Release)..."
        cmake --build . --config Release
    fi
    
    print_msg "$GREEN" "✅ Backend build complete"
}

# Cleanup function for graceful shutdown
cleanup() {
    print_msg "$YELLOW" "🧹 Shutting down services..."
    
    # Kill frontend if running
    if [ ! -z "$FRONTEND_PID" ] && ps -p $FRONTEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping frontend server (PID: $FRONTEND_PID)..."
        kill $FRONTEND_PID 2>/dev/null || true
    fi
    
    # Kill backend if running
    if [ ! -z "$BACKEND_PID" ] && ps -p $BACKEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping backend (PID: $BACKEND_PID)..."
        kill $BACKEND_PID 2>/dev/null || true
    fi
    
    # Kill any remaining node processes from this script
    pkill -P $$ 2>/dev/null || true
    
    print_msg "$GREEN" "✅ Shutdown complete"
    exit 0
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Run development mode (single command - runs everything!)
run_dev() {
    print_header "Starting Development Environment"
    
    print_msg "$MAGENTA" "🚀 Starting with single command..."
    print_msg "$BLUE" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    # Build backend first
    build_backend "dev"
    
    # Start frontend dev server in background
    print_msg "$YELLOW" "💻 Starting Next.js dev server..."
    cd "$FRONTEND_DIR"
    npm run dev > /dev/null 2>&1 &
    FRONTEND_PID=$!
    cd "$PROJECT_ROOT"
    
    # Wait for frontend to be ready
    print_msg "$YELLOW" "⏳ Waiting for frontend server..."
    local max_attempts=30
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s http://localhost:3000 > /dev/null 2>&1; then
            print_msg "$GREEN" "✅ Frontend ready at http://localhost:3000"
            break
        fi
        sleep 1
        attempt=$((attempt + 1))
        if [ $attempt -eq $max_attempts ]; then
            print_msg "$RED" "❌ Frontend failed to start"
            cleanup
            exit 1
        fi
    done
    
    # Start backend (Qt app with WebEngine)
    print_msg "$YELLOW" "🖥️  Starting Qt application..."
    print_msg "$BLUE" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$GREEN" "🎉 Everything is ready!"
    print_msg "$MAGENTA" "   Frontend: http://localhost:3000"
    print_msg "$MAGENTA" "   Backend:  http://localhost:8080"
    print_msg "$MAGENTA" "   Qt App:   Opening now..."
    print_msg "$BLUE" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    print_msg "$YELLOW" "💡 Press Ctrl+C to stop all services"
    echo ""
    
    # Run the Qt application (this will block until app closes)
    cd "$BACKEND_BUILD_DIR"
    if [ -f "bin/TrainSimulationApp" ]; then
        ./bin/TrainSimulationApp --frontend=http://localhost:3000
    elif [ -f "TrainSimulationApp" ]; then
        ./TrainSimulationApp --frontend=http://localhost:3000
    else
        print_msg "$RED" "❌ Error: TrainSimulationApp executable not found"
        cleanup
        exit 1
    fi
    
    # When Qt app closes, cleanup will run automatically
}

# Build production
build_prod() {
    print_header "Building Production Release"
    
    build_frontend "prod"
    build_backend "prod"
    
    print_msg "$GREEN" "🎉 Production build complete!"
    print_msg "$BLUE" "📦 Executable location:"
    
    if [ -f "$BACKEND_BUILD_DIR/bin/TrainSimulationApp" ]; then
        print_msg "$BLUE" "   $BACKEND_BUILD_DIR/bin/TrainSimulationApp"
    elif [ -f "$BACKEND_BUILD_DIR/TrainSimulationApp" ]; then
        print_msg "$BLUE" "   $BACKEND_BUILD_DIR/TrainSimulationApp"
    fi
}

# Just run the app (assumes already built)
run_app() {
    print_header "Running Application"
    
    # Check if backend is built
    if [ ! -f "$BACKEND_BUILD_DIR/bin/TrainSimulationApp" ] && [ ! -f "$BACKEND_BUILD_DIR/TrainSimulationApp" ]; then
        print_msg "$YELLOW" "⚠️  Backend not built yet. Building now..."
        build_backend "dev"
    fi
    
    # Start frontend dev server in background
    print_msg "$YELLOW" "💻 Starting Next.js dev server..."
    cd "$FRONTEND_DIR"
    npm run dev > /dev/null 2>&1 &
    FRONTEND_PID=$!
    cd "$PROJECT_ROOT"
    
    # Wait for frontend
    print_msg "$YELLOW" "⏳ Waiting for frontend server..."
    local max_attempts=30
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s http://localhost:3000 > /dev/null 2>&1; then
            print_msg "$GREEN" "✅ Frontend ready at http://localhost:3000"
            break
        fi
        sleep 1
        attempt=$((attempt + 1))
    done
    
    print_msg "$GREEN" "🚀 Starting application..."
    echo ""
    print_msg "$YELLOW" "💡 Press Ctrl+C to stop all services"
    echo ""
    
    # Run Qt app
    cd "$BACKEND_BUILD_DIR"
    if [ -f "bin/TrainSimulationApp" ]; then
        ./bin/TrainSimulationApp --frontend=http://localhost:3000
    elif [ -f "TrainSimulationApp" ]; then
        ./TrainSimulationApp --frontend=http://localhost:3000
    fi
}

# Show usage
show_usage() {
    echo "Usage: ./build.sh [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  dev      - Build and run in development mode (default) ⭐"
    echo "  run      - Just run (assumes already built)"
    echo "  install  - Install all dependencies"
    echo "  prod     - Build production release"
    echo "  clean    - Clean all build artifacts"
    echo "  help     - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./build.sh          # Build and run dev (SINGLE COMMAND!)"
    echo "  ./build.sh install  # Install dependencies"
    echo "  ./build.sh run      # Just run (if already built)"
    echo "  ./build.sh prod     # Build production"
    echo "  ./build.sh clean    # Clean builds"
    echo ""
    echo "⭐ Most common: ./build.sh dev (does everything!)"
}

# Main script logic
main() {
    local command=${1:-dev}
    
    print_msg "$BLUE" "🚂 Train Simulation App - Build System"
    print_msg "$BLUE" "📁 Working from: $PROJECT_ROOT"
    echo ""
    
    case $command in
        dev)
            check_prerequisites
            run_dev
            ;;
        run)
            check_prerequisites
            run_app
            ;;
        install)
            check_prerequisites
            install_dependencies
            ;;
        prod)
            check_prerequisites
            build_prod
            ;;
        clean)
            clean_build
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_msg "$RED" "❌ Unknown command: $command"
            echo ""
            show_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
