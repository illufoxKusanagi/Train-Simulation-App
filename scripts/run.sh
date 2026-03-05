#!/usr/bin/env bash
# Cross-platform runner for Train Simulation App
# Works on Linux and can be adapted for Windows (use run.ps1 for PowerShell)
# Based on patterns from dev.sh but standalone

set -e

# Colors for output (referenced from dev.sh)
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Detect project root (go up from scripts/ directory)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BACKEND_DIR="$PROJECT_ROOT/backend"
FRONTEND_DIR="$PROJECT_ROOT/frontend"
BACKEND_BUILD_DIR="$BACKEND_DIR/build"

# PID tracking for cleanup
FRONTEND_PID=""
BACKEND_PID=""

# Print colored message (pattern from dev.sh)
print_msg() {
    local color=$1
    local msg=$2
    echo -e "${color}${msg}${NC}"
}

# Print section header (pattern from dev.sh)
print_header() {
    echo ""
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$CYAN" "  $1"
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

# Check if command exists (pattern from dev.sh)
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_msg "$RED" "❌ Error: $1 is not installed"
        return 1
    fi
    return 0
}

# Cleanup function (pattern from dev.sh)
cleanup() {
    print_msg "$YELLOW" "🧹 Shutting down services..."
    
    if [ ! -z "$FRONTEND_PID" ] && ps -p $FRONTEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping Next.js dev server (PID: $FRONTEND_PID)..."
        kill $FRONTEND_PID 2>/dev/null || true
    fi
    
    if [ ! -z "$BACKEND_PID" ] && ps -p $BACKEND_PID > /dev/null 2>&1; then
        print_msg "$YELLOW" "Stopping backend (PID: $BACKEND_PID)..."
        kill $BACKEND_PID 2>/dev/null || true
    fi
    
    pkill -P $$ 2>/dev/null || true
    print_msg "$GREEN" "✅ Shutdown complete"
}

trap cleanup EXIT INT TERM

# Check prerequisites (pattern from dev.sh)
check_prerequisites() {
    print_header "Checking Prerequisites"
    print_msg "$YELLOW" "Checking required tools..."
    
    local all_ok=true
    check_command cmake || all_ok=false
    check_command node || all_ok=false
    check_command npm || all_ok=false
    
    if [ "$all_ok" = true ]; then
        print_msg "$GREEN" "✅ All prerequisites satisfied"
        return 0
    else
        print_msg "$RED" "❌ Missing required tools"
        exit 1
    fi
}

# Build backend (pattern from dev.sh)
build_backend() {
    local mode=${1:-Debug}
    print_header "Building Backend (Qt + QHttpServer)"
    
    mkdir -p "$BACKEND_BUILD_DIR"
    cd "$BACKEND_BUILD_DIR"
    
    print_msg "$YELLOW" "🔨 Configuring CMake ($mode)..."
    cmake -DCMAKE_BUILD_TYPE=$mode ..
    
    print_msg "$YELLOW" "🔨 Building Qt application ($mode)..."
    cmake --build . --config $mode
    
    print_msg "$GREEN" "✅ Backend build complete"
    cd "$PROJECT_ROOT"
}

# Install frontend dependencies (pattern from dev.sh)
install_frontend_deps() {
    if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
        print_msg "$YELLOW" "📦 Installing frontend dependencies..."
        cd "$PROJECT_ROOT"
        npm install --workspace=frontend
        cd "$PROJECT_ROOT"
    fi
}

# Start Next.js dev server (pattern from dev.sh)
start_nextjs_dev() {
    print_msg "$YELLOW" "💻 Starting Next.js dev server..."
    
    # Kill anything running on port 3254 (Frontend) and 3000 (Default)
    if command -v fuser &> /dev/null; then
        fuser -k 3254/tcp > /dev/null 2>&1 || true
        fuser -k 3000/tcp > /dev/null 2>&1 || true
    elif command -v lsof &> /dev/null; then
         lsof -ti:3254 | xargs -r kill -9 || true
         lsof -ti:3000 | xargs -r kill -9 || true
    fi
    
    cd "$FRONTEND_DIR"
    # Redirect output to log file for debugging
    npm run dev > "$PROJECT_ROOT/frontend.log" 2>&1 &
    FRONTEND_PID=$!
    print_msg "$BLUE" "📝 Frontend logs redirected to: $PROJECT_ROOT/frontend.log"
    cd "$PROJECT_ROOT"
    
    # Wait for Next.js to be ready
    print_msg "$YELLOW" "⏳ Waiting for Next.js dev server..."
    local max_attempts=30
    local attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s -o /dev/null -w "%{http_code}" http://127.0.0.1:3254 | grep -q "200"; then
            print_msg "$GREEN" "✅ Next.js dev server ready at http://127.0.0.1:3254"
            return 0
        fi
        sleep 1
        attempt=$((attempt + 1))
    done
    
    print_msg "$RED" "❌ Next.js dev server failed to start"
    if [ -f "$PROJECT_ROOT/frontend.log" ]; then
        print_msg "$RED" "Last 20 lines of frontend log:"
        tail -n 20 "$PROJECT_ROOT/frontend.log"
    fi
    return 1
}

# Run development mode (pattern from dev.sh)
run_dev() {
    print_header "Starting Development Environment"
    print_msg "$MAGENTA" "🚀 Starting application in development mode..."
    
    # Install dependencies if needed
    install_frontend_deps
    
    # Build backend
    build_backend "Debug"
    
    # Start Next.js dev server
    start_nextjs_dev || exit 1
    
    # Display ready message
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    print_msg "$GREEN" "🎉 Application ready!"
    print_msg "$MAGENTA" "   Backend API: http://localhost:8080"
    print_msg "$MAGENTA" "   Frontend:    http://localhost:3254 (Next.js dev server)"
    print_msg "$MAGENTA" "   Qt Window:   Loading frontend..."
    print_msg "$CYAN" "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    print_msg "$YELLOW" "💡 Press Ctrl+C to stop all services"
    echo ""
    
    # Run Qt application (blocks until closed)
    # Prefix all output with [BACKEND] for clarity
    cd "$BACKEND_BUILD_DIR"
    if [ -f "bin/TrainSimulationApp" ]; then
        export FONTCONFIG_FILE=/etc/fonts/fonts.conf
        export FONTCONFIG_PATH=/etc/fonts
        export QT_LOGGING_RULES="*.debug=true"
        export QT_FORCE_STDERR_LOGGING=1
        print_msg "$BLUE" "🔍 Backend logs will appear below with [BACKEND] prefix:"
        echo ""
        # Use unbuffer if available for immediate output, otherwise use stdbuf
        if command -v unbuffer &> /dev/null; then
            unbuffer ./bin/TrainSimulationApp 2>&1 | sed 's/^/[BACKEND] /'
        elif command -v stdbuf &> /dev/null; then
            stdbuf -oL -eL ./bin/TrainSimulationApp 2>&1 | sed 's/^/[BACKEND] /'
        else
            ./bin/TrainSimulationApp 2>&1 | sed 's/^/[BACKEND] /'
        fi
    else
        print_msg "$RED" "❌ Error: backend executable not found"
        print_msg "$YELLOW" "Expected: $BACKEND_BUILD_DIR/bin/TrainSimulationApp"
        exit 1
    fi
}

# Build production (pattern from dev.sh)
build_production() {
    print_header "Building Production Release"
    
    # Install deps
    install_frontend_deps
    
    # Build frontend
    print_msg "$YELLOW" "🔨 Building Next.js for production..."
    cd "$FRONTEND_DIR"
    npm run build
    print_msg "$GREEN" "✅ Frontend build complete"
    
    # Build backend
    build_backend "Release"
    
    print_msg "$GREEN" "🎉 Production build complete!"
    print_msg "$BLUE" "📦 Executable: $BACKEND_BUILD_DIR/bin/TrainSimulationApp"
}

# Run production (pattern from dev.sh)
run_production() {
    print_header "Running Production Build"
    
    if [ ! -f "$BACKEND_BUILD_DIR/bin/TrainSimulationApp" ]; then
        print_msg "$YELLOW" "⚠️  Production build not found. Building now..."
        build_production
    fi
    
    print_msg "$GREEN" "🚀 Starting application (Production)..."
    cd "$BACKEND_BUILD_DIR"
    export FONTCONFIG_FILE=/etc/fonts/fonts.conf
    export FONTCONFIG_PATH=/etc/fonts
    ./bin/TrainSimulationApp
}

# Clean build artifacts (pattern from dev.sh)
clean_build() {
    print_header "Cleaning Build Artifacts"
    
    if [ -d "$BACKEND_BUILD_DIR" ]; then
        print_msg "$YELLOW" "🧹 Cleaning backend build..."
        rm -rf "$BACKEND_BUILD_DIR"
    fi
    
    if [ -d "$FRONTEND_DIR/.next" ]; then
        print_msg "$YELLOW" "🧹 Cleaning frontend build..."
        rm -rf "$FRONTEND_DIR/.next" "$FRONTEND_DIR/out"
    fi
    
    print_msg "$GREEN" "✅ Clean complete"
}

# Package for Linux (AppImage)
package_appimage() {
    print_header "Packaging AppImage (Linux)"
    
    # Ensure production build exists
    if [ ! -f "$BACKEND_BUILD_DIR/bin/TrainSimulationApp" ]; then
        print_msg "$YELLOW" "⚠️  Release build not found. Building now..."
        build_production
    fi
    
    local dist_dir="$PROJECT_ROOT/dist"
    local app_dir="$dist_dir/AppDir"
    
    # Clean previous package attempt
    rm -rf "$dist_dir"
    mkdir -p "$app_dir/usr/bin"
    mkdir -p "$app_dir/usr/share/train-simulation-app"
    mkdir -p "$app_dir/usr/share/applications"
    mkdir -p "$app_dir/usr/share/icons/hicolor/256x256/apps"
    
    print_msg "$YELLOW" "📦 Assembling AppDir structure..."
    
    # Copy binaries and assets
    cp "$BACKEND_BUILD_DIR/bin/TrainSimulationApp" "$app_dir/usr/bin/train-simulation-app"
    cp -r "$FRONTEND_DIR/out"/* "$app_dir/usr/share/train-simulation-app/"
    
    # Create valid desktop entry
    cat > "$app_dir/usr/share/applications/train-simulation-app.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Train Simulation App
Comment=Train Simulation Application
Exec=train-simulation-app
Icon=utilities-terminal
Categories=Education;Simulation;
Terminal=false
EOF

    # Create dummy icon if missing (required by linuxdeployqt)
    touch "$app_dir/usr/share/icons/hicolor/256x256/apps/utilities-terminal.png"
    
    # Check for linuxdeployqt
    local deploy_tool="linuxdeployqt-continuous-x86_64.AppImage"
    if [ ! -f "$PROJECT_ROOT/$deploy_tool" ]; then
        print_msg "$YELLOW" "⬇️  Downloading linuxdeployqt..."
        wget -q "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/$deploy_tool" -O "$PROJECT_ROOT/$deploy_tool"
        chmod +x "$PROJECT_ROOT/$deploy_tool"
    fi
    
    print_msg "$YELLOW" "🎁 Generating AppImage..."
    
    # Run linuxdeployqt
    # Note: We assume qmake is in PATH or automatically found. 
    # If standard install, it should work. check_prerequisites verifies cmake/npm but not qmake.
    
    # Using -appimage -unsupported-allow-new-glibc to maximize compatibility on newer dev machines
    export VERSION="1.0.0" # Required by linuxdeployqt
    "$PROJECT_ROOT/$deploy_tool" "$app_dir/usr/share/applications/train-simulation-app.desktop" \
        -appimage -unsupported-allow-new-glibc \
        -executable="$app_dir/usr/bin/train-simulation-app"
        
    # Move artifact
    find "$PROJECT_ROOT" -maxdepth 1 -name "Train_Simulation_App-*.AppImage" -exec mv {} "$dist_dir/" \;
    
    if ls "$dist_dir"/*.AppImage 1> /dev/null 2>&1; then
        print_msg "$GREEN" "🎉 AppImage created successfully!"
        print_msg "$BLUE" "📂 Location: $(ls $dist_dir/*.AppImage)"
    else
        print_msg "$RED" "❌ AppImage creation failed. Check logs above."
        exit 1
    fi
}

# Show usage
show_usage() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  dev        - Build and run in development mode (default)"
    echo "  build      - Build production release"
    echo "  run        - Run production build"
    echo "  package    - Build and package as AppImage (Linux)"
    echo "  clean      - Clean all build artifacts"
    echo "  help       - Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 dev     # Start development"
    echo "  $0 package # Create AppImage"
}

# Main logic
main() {
    local command=${1:-dev}
    
    print_msg "$CYAN" "🚂 Train Simulation App - Cross-Platform Runner"
    print_msg "$CYAN" "📁 Project: $PROJECT_ROOT"
    echo ""
    
    case $command in
        dev|development)
            check_prerequisites
            run_dev
            ;;
        build|prod|production)
            check_prerequisites
            build_production
            ;;
        run|start)
            check_prerequisites
            run_production
            ;;
        package|dist)
            check_prerequisites
            package_appimage
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

main "$@"

