#!/bin/bash

# Build script for all MML_Visualizers (FLTK and Qt) on Linux
# Usage: ./build_all.sh [clean]
#   clean - Remove all build directories before building

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLEAN_BUILD=false

# Color codes
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check for clean parameter
if [ "$1" == "clean" ]; then
    CLEAN_BUILD=true
    echo -e "${YELLOW}Clean build requested - will remove all build directories${NC}"
fi

# FLTK visualizers array
FLTK_VISUALIZERS=(
    "MML_RealFunctionVisualizer"
    "MML_ParametricCurve2D_Visualizer"
    "MML_ParticleVisualizer2D"
    "MML_VectorField2D_Visualizer"
)

# Qt visualizers array
QT_VISUALIZERS=(
    "MML_RealFunctionVisualizer"
    "MML_ParametricCurve3D_Visualizer"
    "MML_ParticleVisualizer3D"
    "MML_ScalarFunction2D_Visualizer"
    "MML_VectorField3D_Visualizer"
)

# Get number of CPU cores for parallel build
NPROC=$(nproc)

echo "========================================"
echo "MML_Visualizers Build Script"
echo "========================================"
echo "Build directory: $SCRIPT_DIR"
echo "Parallel jobs: $NPROC"
echo "Clean build: $CLEAN_BUILD"
echo ""

# Track build statistics
TOTAL_BUILT=0
TOTAL_FAILED=0
FAILED_BUILDS=()

# Function to build FLTK visualizers
build_fltk_visualizers() {
    echo -e "${BLUE}========================================"
    echo "Building FLTK Visualizers (4 total)"
    echo -e "========================================${NC}"
    echo ""

    cd "$SCRIPT_DIR/FLTK"

    # Clean if requested
    if [ "$CLEAN_BUILD" == true ]; then
        echo -e "${YELLOW}Cleaning FLTK build directory...${NC}"
        rm -rf build
        echo ""
    fi

    # Create build directory
    mkdir -p build
    cd build

    echo -e "${GREEN}Configuring FLTK visualizers...${NC}"
    if ! cmake ..; then
        echo -e "${RED}✗ FLTK CMake configuration failed${NC}"
        return 1
    fi

    echo -e "${GREEN}Building FLTK visualizers...${NC}"
    if ! cmake --build . -j"$NPROC"; then
        echo -e "${RED}✗ FLTK build failed${NC}"
        return 1
    fi

    echo -e "${GREEN}✓ All FLTK visualizers built successfully${NC}"
    echo ""
    
    # List built binaries
    echo "Built FLTK binaries:"
    ls -lh bin/ | grep -v "^total" | awk '{printf "  %s (%s)\n", $9, $5}'
    echo ""

    return 0
}

# Function to build a single Qt visualizer
build_qt_visualizer() {
    local visualizer=$1
    local build_dir="$SCRIPT_DIR/Qt/$visualizer/build"

    echo "----------------------------------------"
    echo -e "${GREEN}Building: $visualizer${NC}"
    echo "----------------------------------------"

    # Clean if requested
    if [ "$CLEAN_BUILD" == true ]; then
        echo -e "${YELLOW}  Cleaning build directory...${NC}"
        rm -rf "$build_dir"
    fi

    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir"

    # Configure with Qt path detection
    local qt_path="${QT_DIR:-/home/zvanjak/Qt/6.9.3/gcc_64}"
    if ! cmake -DCMAKE_PREFIX_PATH="$qt_path" .. > /dev/null 2>&1; then
        echo -e "${RED}  ✗ CMake configuration failed${NC}"
        return 1
    fi

    # Build
    if ! cmake --build . -j"$NPROC" > /dev/null 2>&1; then
        echo -e "${RED}  ✗ Build failed${NC}"
        return 1
    fi

    # Check if binary was created
    local binary=$(find "$build_dir/bin" -maxdepth 1 -type f -executable 2>/dev/null | head -1)
    if [ -n "$binary" ]; then
        local size=$(du -h "$binary" | cut -f1)
        local binary_name=$(basename "$binary")
        echo -e "${GREEN}  ✓ Built: $binary_name ($size)${NC}"
        return 0
    else
        echo -e "${RED}  ✗ Binary not found${NC}"
        return 1
    fi
}

# Function to build Qt visualizers
build_qt_visualizers() {
    echo -e "${BLUE}========================================"
    echo "Building Qt Visualizers (5 total)"
    echo -e "========================================${NC}"
    echo ""

    local qt_built=0
    local qt_failed=0

    for visualizer in "${QT_VISUALIZERS[@]}"; do
        if build_qt_visualizer "$visualizer"; then
            ((qt_built++))
        else
            ((qt_failed++))
            FAILED_BUILDS+=("Qt/$visualizer")
        fi
    done

    echo ""
    echo -e "${BLUE}Qt Build Summary:${NC}"
    echo -e "  Built: ${GREEN}$qt_built${NC}"
    if [ $qt_failed -gt 0 ]; then
        echo -e "  Failed: ${RED}$qt_failed${NC}"
    fi
    echo ""

    TOTAL_BUILT=$((TOTAL_BUILT + qt_built))
    TOTAL_FAILED=$((TOTAL_FAILED + qt_failed))

    return $qt_failed
}

# Main build process
echo -e "${BLUE}Starting build process...${NC}"
echo ""

# Build FLTK visualizers
if build_fltk_visualizers; then
    TOTAL_BUILT=$((TOTAL_BUILT + 4))
else
    TOTAL_FAILED=$((TOTAL_FAILED + 4))
    FAILED_BUILDS+=("FLTK/All")
fi

# Build Qt visualizers
build_qt_visualizers

# Final summary
echo "========================================"
echo "Build Summary"
echo "========================================"
echo -e "Total built: ${GREEN}$TOTAL_BUILT${NC}"
if [ $TOTAL_FAILED -gt 0 ]; then
    echo -e "Total failed: ${RED}$TOTAL_FAILED${NC}"
    echo ""
    echo "Failed builds:"
    for failed in "${FAILED_BUILDS[@]}"; do
        echo -e "  ${RED}✗${NC} $failed"
    done
    echo ""
    echo -e "${RED}Build completed with errors!${NC}"
    exit 1
else
    echo -e "Total failed: ${GREEN}0${NC}"
    echo ""
    echo -e "${GREEN}All visualizers built successfully!${NC}"
    echo ""
    echo "Next steps:"
    echo "  1. Run update scripts to deploy binaries:"
    echo "     cd FLTK && ./update_apps.sh"
    echo "     cd Qt && ./update_apps.sh"
    echo "  2. Or test individual visualizers using run_*.sh scripts"
fi
echo "========================================"

exit 0
