#!/usr/bin/env bash
set -euo pipefail

# cross-platform runner (bash)
# Usage:
#   ./scripts/run.sh dev        # start frontend dev + backend (dev)
#   ./scripts/run.sh prod       # build frontend + build backend release
#   ./scripts/run.sh build-frontend
#   ./scripts/run.sh build-backend
#   ./scripts/run.sh run-backend
#   ./scripts/run.sh run-frontend
#   ./scripts/run.sh package

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
FRONTEND_DIR="$ROOT_DIR/frontend"
BACKEND_DIR="$ROOT_DIR/backend"
BUILD_DIR="$BACKEND_DIR/build"

function ensure_build_dir() {
  mkdir -p "$BUILD_DIR"
}

function start_frontend_dev() {
  echo "➡️  Starting Next.js frontend (dev)"
  (cd "$FRONTEND_DIR" && npm install && npm run dev) &
  FRONTEND_PID=$!
  echo "Frontend PID: $FRONTEND_PID"
}

function start_backend_dev() {
  echo "➡️  Building & starting Qt backend (dev)"
  ensure_build_dir
  (cd "$BUILD_DIR" && cmake .. && cmake --build .)
  # run the executable (try common output paths)
  if [ -x "$BUILD_DIR/bin/TrainSimulationApp" ]; then
    "$BUILD_DIR/bin/TrainSimulationApp" &
    BACKEND_PID=$!
  elif [ -x "$BUILD_DIR/TrainSimulationApp" ]; then
    "$BUILD_DIR/TrainSimulationApp" &
    BACKEND_PID=$!
  else
    echo "⚠️  Could not find built executable. Check build output in $BUILD_DIR"
  fi
  echo "Backend PID: ${BACKEND_PID:-unknown}"
}

function build_frontend() {
  echo "➡️  Building Next.js frontend (production)"
  (cd "$FRONTEND_DIR" && npm install && npm run build)
}

function build_backend() {
  echo "➡️  Building Qt backend (Release)"
  ensure_build_dir
  (cd "$BUILD_DIR" && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .)
}

function run_frontend_prod() {
  if [ -d "$FRONTEND_DIR/out" ]; then
    echo "➡️  Serving static frontend from frontend/out"
    # A very small static file server using Python
    (cd "$FRONTEND_DIR/out" && python3 -m http.server 3000) &
    FRONTEND_PID=$!
  else
    echo "⚠️  frontend/out not found. Build frontend first."
  fi
}

function usage() {
  sed -n '1,120p' "$0" | sed -n '1,120p'
}

case "${1:-}" in
  dev)
    start_frontend_dev
    start_backend_dev
    wait
    ;;
  prod)
    build_frontend
    build_backend
    ;;
  build-frontend)
    build_frontend
    ;;
  build-backend)
    build_backend
    ;;
  run-backend)
    start_backend_dev
    wait
    ;;
  run-frontend)
    run_frontend_prod
    wait
    ;;
  package)
    echo "➡️  Packaging: build frontend + backend (Release)"
    build_frontend
    build_backend
    echo "Packaging is left to platform-specific packaging scripts (AppImage, windeployqt, etc.)"
    ;;
  *)
    echo "Run script for Train Simulation App"
    echo "Usage: $0 {dev|prod|build-frontend|build-backend|run-backend|run-frontend|package}"
    exit 1
    ;;
esac
