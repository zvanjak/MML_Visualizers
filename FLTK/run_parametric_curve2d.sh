#!/bin/bash

# Script to run MML_ParametricCurve2D_Visualizer_FLTK with all test data files

VISUALIZER="./MML_ParametricCurve2D_Visualizer/build/bin/MML_ParametricCurve2D_Visualizer_FLTK"
DATA_DIR="../data/ParametricCurve2D"

# Check if visualizer executable exists
if [ ! -f "$VISUALIZER" ]; then
    echo "Error: Visualizer not found at $VISUALIZER"
    echo "Please build the project first."
    exit 1
fi

# Check if data directory exists
if [ ! -d "$DATA_DIR" ]; then
    echo "Error: Data directory not found at $DATA_DIR"
    exit 1
fi

echo "Running MML_ParametricCurve2D_Visualizer with test data files..."
echo "Press Ctrl+C to stop"
echo ""

# Loop through all .txt files in the data directory
for datafile in "$DATA_DIR"/*.txt; do
    if [ -f "$datafile" ]; then
        filename=$(basename "$datafile")
        echo "======================================"
        echo "Loading: $filename"
        echo "======================================"
        
        # Run the visualizer with the data file
        "$VISUALIZER" "$datafile"
        
        # Wait a moment between runs
        sleep 0.5
        echo ""
    fi
done

echo "All test files processed."
