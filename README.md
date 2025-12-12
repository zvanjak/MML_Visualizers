# MML_Visualizers
Visualizer apps for MinimalMathLibrary

## Overview

This repository contains visualizer applications for MinimalMathLibrary (MML) in three implementations:
- **WPF**: Windows Presentation Foundation (Windows-only, C#/XAML)
- **FLTK**: Fast Light Toolkit (Cross-platform 2D, C++)
- **Qt**: Qt6 + OpenGL (Cross-platform 2D/3D, C++)

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

## Qt Visualizers (Cross-Platform with OpenGL)

The Qt directory contains modern OpenGL-accelerated implementations:

### 1. MML_RealFunctionVisualizer
- 2D function visualization with OpenGL rendering
- Interactive zoom/pan with mouse controls
- Coordinate axes with labels and grid lines
- Function legend with statistics
- Supports multiple functions simultaneously

### 2. MML_ParametricCurve3D_Visualizer
- Full 3D parametric curve visualization
- Interactive camera controls (rotate, zoom, pan)
- Multiple curves with color coding
- Real-time OpenGL rendering

### 3. MML_ParticleVisualizer3D
- 3D particle simulation visualization
- Animation controls (play/pause/step/reset)
- Bounding box toggle
- Interactive camera controls
- Multiple timestep support

### 4. MML_ScalarFunction2D_Visualizer
- 2D scalar function visualization with heatmaps/contours
- Interactive zoom/pan controls
- Color mode toggle for visualization
- Grid point display option
- Real-time OpenGL rendering

### 5. MML_VectorField3D_Visualizer
- 3D vector field visualization with arrow rendering
- Interactive 3D camera controls
- Color mode for magnitude visualization
- Dynamic vector scaling
- Full 3D navigation

### Building Qt Projects

#### Prerequisites
- CMake 3.16 or higher
- Qt6 (tested with Qt 6.9.3)
- OpenGL support
- C++17 compatible compiler

#### Building

```bash
cd Qt/<visualizer-name>
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..
make
```

Example:
```bash
cd Qt/MML_RealFunctionVisualizer
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/home/user/Qt/6.9.3/gcc_64 ..
make
```

#### Testing Scripts

Automated testing scripts are available:
```bash
cd Qt
./run_real_function.sh          # Test RealFunction with all data files
./run_parametric_curve3d.sh     # Test ParametricCurve3D with all data files
./run_particle_visualizer3d.sh  # Test ParticleVisualizer3D with all data files
./run_scalar_function2d.sh      # Test ScalarFunction2D with all data files
./run_vector_field3d.sh         # Test VectorField3D with all data files
./update_apps.sh                # Deploy built executables
```

## WPF Visualizers (Windows-only)

The WPF directory contains Windows-specific implementations with additional visualizers including 3D variants. These require the .NET Framework and Visual Studio to build.

## Project Structure

```
MML_Visualizers/
├── FLTK/           # Cross-platform 2D visualizers (C++ + FLTK)
├── Qt/             # Cross-platform 2D/3D visualizers (C++ + Qt6 + OpenGL)
├── WPF/            # Windows-only visualizers (C# + XAML)
├── data/           # Centralized test data directory
└── visualizers/    # Deployed executables
    └── linux/
        ├── FLTK/   # FLTK executables
        └── Qt/     # Qt executables
```

## License

See LICENSE file for details.
