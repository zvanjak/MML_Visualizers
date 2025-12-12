#!/bin/bash

# Script to update FLTK visualizer executables in the deployment directory
# Only copies if built versions are newer than deployed versions

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build/bin"
DEPLOY_DIR="${SCRIPT_DIR}/../visualizers/linux/FLTK"

# Array of visualizer executables (with _FLTK suffix on Linux)
VISUALIZERS=(
    "MML_RealFunctionVisualizer_FLTK"
    "MML_ParametricCurve2D_Visualizer_FLTK"
    "MML_ParticleVisualizer2D_FLTK"
    "MML_VectorField2D_Visualizer_FLTK"
)

# Color codes for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================"
echo "FLTK Visualizers Update Script"
echo "========================================"
echo "Build directory: $BUILD_DIR"
echo "Deploy directory: $DEPLOY_DIR"
echo ""

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}Error: Build directory not found: $BUILD_DIR${NC}"
    echo "Please build the project first using:"
    echo "  cd $SCRIPT_DIR && mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Create deploy directory if it doesn't exist
if [ ! -d "$DEPLOY_DIR" ]; then
    echo "Creating deploy directory: $DEPLOY_DIR"
    mkdir -p "$DEPLOY_DIR"
fi

# Counters
updated=0
skipped=0
not_found=0

# Process each visualizer
for app in "${VISUALIZERS[@]}"; do
    src_file="$BUILD_DIR/$app"
    dest_file="$DEPLOY_DIR/$app"
    
    echo "----------------------------------------"
    echo "Processing: $app"
    
    # Check if source file exists
    if [ ! -f "$src_file" ]; then
        echo -e "${RED}  ✗ Not found in build directory${NC}"
        ((not_found++))
        continue
    fi
    
    # Get source file timestamp
    src_time=$(stat -c %Y "$src_file" 2>/dev/null)
    
    # Check if destination exists and compare timestamps
    if [ -f "$dest_file" ]; then
        dest_time=$(stat -c %Y "$dest_file" 2>/dev/null)
        
        if [ "$src_time" -gt "$dest_time" ]; then
            echo -e "${GREEN}  ✓ Copying (built version is newer)${NC}"
            cp -v "$src_file" "$dest_file"
            chmod +x "$dest_file"
            ((updated++))
        else
            echo -e "${YELLOW}  → Skipped (deployed version is up to date)${NC}"
            ((skipped++))
        fi
    else
        echo -e "${GREEN}  ✓ Copying (first deployment)${NC}"
        cp -v "$src_file" "$dest_file"
        chmod +x "$dest_file"
        ((updated++))
    fi
done

# Summary
echo ""
echo "========================================"
echo "Update Summary"
echo "========================================"
echo -e "${GREEN}Updated: $updated${NC}"
echo -e "${YELLOW}Skipped (up to date): $skipped${NC}"
echo -e "${RED}Not found: $not_found${NC}"
echo "========================================"

if [ $updated -gt 0 ]; then
    echo ""
    echo "Deployed executables are in: $DEPLOY_DIR"
fi

exit 0
