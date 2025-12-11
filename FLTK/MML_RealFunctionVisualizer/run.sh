#!/bin/bash

# Run script for MML_RealFunctionVisualizer on Linux

BUILD_DIR="$(cd "$(dirname "$0")/build" && pwd)"
EXECUTABLE="$BUILD_DIR/bin/MML_RealFunctionVisualizer"
DATA_DIR="$(cd "$(dirname "$0")/../../WPF/MML_RealFunctionVisualizer/data" && pwd)"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found at $EXECUTABLE"
    echo "Please build the project first by running:"
    echo "  cd $(dirname "$0")"
    echo "  mkdir -p build && cd build"
    echo "  cmake .. && make"
    exit 1
fi

# If no arguments provided, show available test files
if [ $# -eq 0 ]; then
    echo "Usage: $0 [data_file]"
    echo ""
    echo "Available test files in $DATA_DIR:"
    ls -1 "$DATA_DIR"/*.txt 2>/dev/null | while read file; do
        echo "  $(basename "$file")"
    done
    echo ""
    echo "Examples:"
    echo "  $0 real_func1.txt"
    echo "  $0 multi_real_func.txt"
    echo "  $0 $DATA_DIR/real_func1.txt"
    exit 0
fi

# Run the visualizer
if [ -f "$1" ]; then
    "$EXECUTABLE" "$1"
elif [ -f "$DATA_DIR/$1" ]; then
    "$EXECUTABLE" "$DATA_DIR/$1"
else
    echo "Error: File not found: $1"
    exit 1
fi
