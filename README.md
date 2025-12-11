# MML_Visualizers
Visualizer apps for MinimalMathLibrary

## Overview

This repository contains visualizer applications for MinimalMathLibrary (MML) in two implementations:
- **WPF**: Windows Presentation Foundation (Windows-only)
- **FLTK**: Fast Light Toolkit (Cross-platform: Windows, Linux, macOS)

## FLTK Visualizers (Cross-Platform)

The FLTK directory contains cross-platform implementations of the following visualizers:

### 1. MML_RealFunctionVisualizer
- Visualizes real-valued functions (single and multi-function datasets)
- Supports multiple functions with color-coded legend
- Interactive file loading

### 2. MML_ParametricCurve2D_Visualizer
- Visualizes 2D parametric curves
- Supports multiple curves simultaneously
- Color-coded visualization with legend

### 3. MML_ParticleVisualizer2D
- Animates 2D particle simulations
- Playback controls (Play/Pause/Reset)
- Step-by-step navigation
- Color-coded particles with legend

### 4. MML_VectorField2D_Visualizer
- Visualizes 2D vector fields
- Automatic vector scaling
- Arrow-based representation
- Grid and coordinate system

## Building FLTK Projects

### Prerequisites
- CMake 3.15 or higher
- FLTK library installed
- C++17 compatible compiler

### Installation of FLTK

#### Windows
```bash
vcpkg install fltk:x64-windows
```

#### Linux
```bash
sudo apt-get install libfltk1.3-dev  # Debian/Ubuntu
# or
sudo yum install fltk-devel          # Fedora/RHEL
```

#### macOS
```bash
brew install fltk
```

### Building

Each visualizer has its own build directory. To build a specific visualizer:

```bash
cd FLTK/<visualizer-name>
mkdir build
cd build
cmake ..
cmake --build .
```

Example:
```bash
cd FLTK/MML_ParametricCurve2D_Visualizer
mkdir build
cd build
cmake ..
cmake --build .
```

The executables will be in the `build/bin` directory.

## Usage

All visualizers support command-line file loading:
```bash
./MML_ParametricCurve2D_Visualizer curve1.txt curve2.txt
./MML_ParticleVisualizer2D simulation.txt
./MML_VectorField2D_Visualizer vector_field.txt
```

Or use the "Load File" button for interactive file selection.

## Data Formats

Each visualizer has specific data format requirements. See individual README files in each visualizer directory for detailed format specifications.

## WPF Visualizers (Windows-only)

The WPF directory contains Windows-specific implementations with additional visualizers including 3D variants. These require the .NET Framework and Visual Studio to build.

## License

See LICENSE file for details.
