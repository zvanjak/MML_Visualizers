# Build Instructions for MML FLTK Visualizers

## Quick Start

### Build All Visualizers

From the FLTK directory:

```bash
mkdir build
cd build
cmake -f ../MML_CMakeLists.txt ..
cmake --build .
```

### Build Individual Visualizer

From a specific visualizer directory:

```bash
cd MML_ParametricCurve2D_Visualizer
mkdir build
cd build
cmake ..
cmake --build .
```

## Platform-Specific Instructions

### Windows (with Visual Studio)

```bash
# Using vcpkg for FLTK
vcpkg install fltk:x64-windows

# Build all visualizers
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake -f ../MML_CMakeLists.txt ..
cmake --build . --config Release
```

### Linux

```bash
# Install FLTK
sudo apt-get install libfltk1.3-dev

# Build all
mkdir build && cd build
cmake -f ../MML_CMakeLists.txt ..
cmake --build .
```

### macOS

```bash
# Install FLTK
brew install fltk

# Build all
mkdir build && cd build
cmake -f ../MML_CMakeLists.txt ..
cmake --build .
```

## Running the Executables

After building, executables are in the `build/bin` directory:

```bash
cd build/bin

# Run with test data from WPF directories
./MML_ParametricCurve2D_Visualizer ../../WPF/MML_ParametricCurve2D_Visualizer/data/curve1.txt
./MML_ParticleVisualizer2D ../../WPF/MML_ParticleVisualizer2D/data/SimData.txt
./MML_VectorField2D_Visualizer ../../WPF/MML_VectorField2D_Visualizer/data/example3_wpf_vector_field_2d.txt
```

## Troubleshooting

### CMake cannot find FLTK

Try specifying FLTK location explicitly:
```bash
cmake -DFLTK_DIR=/path/to/fltk/lib/cmake ..
```

### Windows: FLTK libraries not found

Make sure vcpkg integration is set up:
```bash
vcpkg integrate install
```

Then use the toolchain file when running cmake.

### Linux: Missing X11 dependencies

```bash
sudo apt-get install libx11-dev libxext-dev libxft-dev libxinerama-dev libxcursor-dev libxi-dev
```
