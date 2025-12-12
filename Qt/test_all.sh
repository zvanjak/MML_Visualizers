#!/bin/bash

# Quick test script to verify all Qt visualizers are working

set +e  # Don't exit on error

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Testing all Qt MML Visualizers..."
echo "===================================="
echo ""

# Set MML_PROJECT_PATH if not already set
if [ -z "$MML_PROJECT_PATH" ]; then
    export MML_PROJECT_PATH="$(cd "$SCRIPT_DIR/.." && pwd)"
    echo "Set MML_PROJECT_PATH=$MML_PROJECT_PATH"
    echo ""
fi

# Test each visualizer with a short timeout
test_visualizer() {
    local name=$1
    local exe=$2
    local datafile=$3
    
    printf "Testing %-40s " "$name..."
    
    if [ ! -f "$exe" ]; then
        echo "✗ Executable not found"
        return 1
    fi
    
    if [ ! -f "$datafile" ]; then
        echo "✗ Data file not found"
        return 1
    fi
    
    # Run with 1 second timeout - if it launches, it works
    timeout 1 "$SCRIPT_DIR/qt_run_wrapper.sh" "$exe" "$datafile" > /dev/null 2>&1
    local exit_code=$?
    
    # Exit code 124 = timeout (good - app launched)
    # Exit code 143 = SIGTERM from timeout (good - app launched)
    if [ $exit_code -eq 124 ] || [ $exit_code -eq 143 ]; then
        echo "✓ Working"
        return 0
    else
        echo "✗ Failed (exit code: $exit_code)"
        return 1
    fi
}

# Test all visualizers
test_visualizer \
    "Parametric Curve 3D" \
    "$SCRIPT_DIR/MML_ParametricCurve3D_Visualizer/build/bin/MML_ParametricCurve3D_Visualizer_Qt" \
    "$MML_PROJECT_PATH/data/ParametricCurve3D/example3_wpf_lorentz_system1.txt"

test_visualizer \
    "Particle Visualizer 3D" \
    "$SCRIPT_DIR/MML_ParticleVisualizer3D/build/bin/MML_ParticleVisualizer3D_Qt" \
    "$MML_PROJECT_PATH/data/ParticleVisualizer3D/air_as_ideal_gas.txt"

test_visualizer \
    "Real Function Visualizer" \
    "$SCRIPT_DIR/MML_RealFunctionVisualizer/build/bin/MML_RealFunctionVisualizer_Qt" \
    "$MML_PROJECT_PATH/data/RealFunction/real_func1.txt"

test_visualizer \
    "Scalar Function 2D" \
    "$SCRIPT_DIR/MML_ScalarFunction2D_Visualizer/build/bin/MML_ScalarFunction2D_Visualizer_Qt" \
    "$MML_PROJECT_PATH/data/ScalarFunction2D/example3_wpf_surface1.txt"

test_visualizer \
    "Vector Field 3D" \
    "$SCRIPT_DIR/MML_VectorField3D_Visualizer/build/bin/MML_VectorField3D_Visualizer_Qt" \
    "$MML_PROJECT_PATH/data/VectorFieldVisualizer3D/vector_field.txt"

echo ""
echo "===================================="
echo "Testing complete!"
echo ""
echo "To run visualizers:"
echo "  ./run_menu.sh - Interactive menu"
echo "  See README_LINUX.md for more options"
