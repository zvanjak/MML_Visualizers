#!/bin/bash

# Convenient script to run any Qt visualizer with sample data

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "MML Visualizers - Qt Edition"
echo "=============================="
echo ""
echo "Available visualizers:"
echo "  1) Parametric Curve 3D"
echo "  2) Particle Visualizer 3D"
echo "  3) Real Function"
echo "  4) Scalar Function 2D"
echo "  5) Vector Field 3D"
echo ""

read -p "Select visualizer (1-5): " choice

case $choice in
    1)
        echo "Running Parametric Curve 3D Visualizer..."
        "$SCRIPT_DIR/run_parametric_curve3d.sh"
        ;;
    2)
        echo "Running Particle Visualizer 3D..."
        "$SCRIPT_DIR/run_particle_visualizer3d.sh"
        ;;
    3)
        echo "Running Real Function Visualizer..."
        "$SCRIPT_DIR/run_real_function.sh"
        ;;
    4)
        echo "Running Scalar Function 2D Visualizer..."
        "$SCRIPT_DIR/run_scalar_function2d.sh"
        ;;
    5)
        echo "Running Vector Field 3D Visualizer..."
        "$SCRIPT_DIR/run_vector_field3d.sh"
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac
