#!/usr/bin/env bash
# 빌드하고 전체 테스트를 돌린다. DB가 떠 있고 스키마가 적용돼 있어야 한다.
set -euo pipefail

cd "$(dirname "$0")/.."
BUILD_DIR="${BUILD_DIR:-build}"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" -j
ctest --test-dir "$BUILD_DIR" --output-on-failure
