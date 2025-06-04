#!/bin/bash
set -e
BUILD_DIR="build"
REPORT_DIR="coverage"

rm -rf "$BUILD_DIR" "$REPORT_DIR" coverage.info
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_C_FLAGS="--coverage"
cmake --build "$BUILD_DIR" --target test -j $(nproc)
./bin/test "$@"

lcov --capture --directory "$BUILD_DIR" --output-file coverage.info
lcov --remove coverage.info '/usr/*' -o coverage.info
mkdir -p "$REPORT_DIR"
genhtml coverage.info --output-directory "$REPORT_DIR" > /dev/null

echo "Coverage report generated in $REPORT_DIR/index.html"
