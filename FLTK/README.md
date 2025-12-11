# FLTK Visualizers - Cross-Platform Build Guide

This directory contains cross-platform visualizer applications built with FLTK (Fast Light Toolkit).

## Available Visualizers

1. **MML_RealFunctionVisualizer** - Visualize real-valued functions
2. **MML_ParametricCurve2D_Visualizer** - Visualize 2D parametric curves
3. **MML_ParticleVisualizer2D** - Animate 2D particle simulations
4. **MML_VectorField2D_Visualizer** - Visualize 2D vector fields

## Prerequisites

### All Platforms
- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- FLTK library (1.3.x or 1.4.x)

### Installing FLTK

#### Windows
Using vcpkg (recommended):
```bash
vcpkg install fltk:x64-windows
```

Or download from: https://www.fltk.org/software.php

#### Linux (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install libfltk1.3-dev
```

#### Linux (Fedora/RHEL/CentOS)
```bash
sudo yum install fltk-devel
```

#### macOS
```bash
brew install fltk
```

## Building

### Option 1: Build Individual Visualizer

```bash
cd MML_<VisualizerName>
mkdir build
cd build
cmake ..
cmake --build .
```

### Option 2: Build All at Once (if master CMakeLists.txt exists)

From the FLTK directory:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Configuration Options

For Release build (optimized):
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

For Debug build:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## Running the Applications

After building, executables are located in the `build/bin` directory:

```bash
# From the build directory
./bin/MML_ParametricCurve2D_Visualizer [datafile1.txt] [datafile2.txt] ...
./bin/MML_ParticleVisualizer2D [simulation.txt]
./bin/MML_VectorField2D_Visualizer [vectorfield.txt]
./bin/MML_RealFunctionVisualizer [function.txt] ...
```

## Troubleshooting

### FLTK Not Found
If CMake cannot find FLTK, specify the FLTK path:
```bash
cmake -DFLTK_DIR=/path/to/fltk ..
```

### Windows Specific
If using vcpkg, make sure to use the vcpkg toolchain:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake ..
```

### Linux - Missing Dependencies
If you get linking errors, install additional dependencies:
```bash
sudo apt-get install libx11-dev libxext-dev libxft-dev libxinerama-dev libxcursor-dev
```

## Data Files

Sample data files are available in the corresponding WPF visualizer directories:
- `WPF/MML_ParametricCurve2D_Visualizer/data/`
- `WPF/MML_ParticleVisualizer2D/data/`
- `WPF/MML_VectorField2D_Visualizer/data/`

## Features

All visualizers include:
- ✅ Cross-platform compatibility
- ✅ Interactive file loading
- ✅ Real-time rendering
- ✅ Command-line argument support
- ✅ Automatic coordinate system scaling

## Performance Notes

FLTK is lightweight and fast. These visualizers can handle:
- MML_ParametricCurve2D: 10,000+ points per curve
- MML_ParticleVisualizer2D: 1,000+ particles
- MML_VectorField2D: 10,000+ vectors

## Documentation

See individual README.md files in each visualizer directory for:
- Detailed data format specifications
- Usage examples
- Feature descriptions
