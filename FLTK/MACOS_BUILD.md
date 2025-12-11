# Building MML Visualizers on macOS

This guide explains how to build the MML Visualizers FLTK implementation on macOS.

## Prerequisites

### 1. Install Xcode Command Line Tools

```bash
xcode-select --install
```

### 2. Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 3. Install FLTK

Using Homebrew:

```bash
brew install fltk
```

### 4. Install CMake

```bash
brew install cmake
```

## Building the Project

### Option 1: Build All Visualizers

From the FLTK directory:

```bash
cd /path/to/MML_Visualizers/FLTK
mkdir build
cd build
cmake ..
make -j4
```

All executables will be in the `build/bin/` directory.

### Option 2: Build Individual Visualizers

You can also build each visualizer separately:

```bash
# Real Function Visualizer
cd MML_RealFunctionVisualizer
mkdir build && cd build
cmake .. && make

# Parametric Curve 2D Visualizer
cd MML_ParametricCurve2D_Visualizer
mkdir build && cd build
cmake .. && make

# Particle Visualizer 2D
cd MML_ParticleVisualizer2D
mkdir build && cd build
cmake .. && make

# Vector Field 2D Visualizer
cd MML_VectorField2D_Visualizer
mkdir build && cd build
cmake .. && make
```

## Running the Applications

### From Build Directory

```bash
cd build/bin
./MML_RealFunctionVisualizer ../../../data/RealFunction/real_func1.txt
./MML_ParametricCurve2D_Visualizer ../../../data/ParametricCurve2D/curve1.txt
./MML_ParticleVisualizer2D ../../../data/ParticleVisualizer2D/SimData.txt
./MML_VectorField2D_Visualizer ../../../data/VectorFieldVisualizer2D/example3_wpf_vector_field_2d.txt
```

## macOS-Specific Features

The CMakeLists.txt files are configured to:

- Automatically detect macOS and link required frameworks (Cocoa, OpenGL)
- Set proper RPATH for library dependencies
- Support both Intel and Apple Silicon (M1/M2/M3) processors
- Create native macOS executables

## Troubleshooting

### FLTK Not Found

If CMake cannot find FLTK, you can specify the FLTK installation path:

```bash
cmake -DFLTK_DIR=/opt/homebrew/lib/cmake/fltk ..
```

Or for Intel Macs:

```bash
cmake -DFLTK_DIR=/usr/local/lib/cmake/fltk ..
```

### OpenGL Warnings

macOS may show deprecation warnings for OpenGL. These can be ignored as FLTK still uses OpenGL for rendering.

### Retina Display Support

FLTK automatically handles Retina displays. No additional configuration is needed.

## Creating App Bundles (Optional)

To create proper macOS app bundles (.app), uncomment the `MACOSX_BUNDLE` settings in the individual CMakeLists.txt files and rebuild.

## Notes

- The build process is the same on both Intel and Apple Silicon Macs
- All visualizers are tested to work with FLTK 1.3.8+
- macOS 10.15 (Catalina) or later is recommended
