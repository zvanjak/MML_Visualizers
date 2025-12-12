#!/bin/bash

# Build all Qt visualizers on Linux

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Building all Qt MML Visualizers..."
echo "===================================="
echo ""

# Array of visualizers to build
VISUALIZERS=(
    "MML_ParametricCurve3D_Visualizer"
    "MML_ParticleVisualizer3D"
    "MML_RealFunctionVisualizer"
    "MML_ScalarFunction2D_Visualizer"
    "MML_VectorField3D_Visualizer"
)

# Build each visualizer
for viz in "${VISUALIZERS[@]}"; do
    echo "Building $viz..."
    cd "$SCRIPT_DIR/$viz"
    
    # Create build directory if it doesn't exist
    mkdir -p build
    cd build
    
    # Configure and build
    cmake .. > /dev/null 2>&1
    make -j$(nproc) > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo "  ✓ $viz built successfully"
    else
        echo "  ✗ $viz build failed"
        exit 1
    fi
    echo ""
done

echo "===================================="
echo "All visualizers built successfully!"
echo ""
echo "To run visualizers:"
echo "  ./run_menu.sh              - Interactive menu"
echo "  ./run_parametric_curve3d.sh - Run with all test data"
echo "  Or run individual executables with data files as arguments"
