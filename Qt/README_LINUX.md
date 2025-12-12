# MML Visualizers - Qt Edition (Linux)

Cross-platform 3D and 2D visualizers built with Qt6 and OpenGL, now fully working on Linux!

## Quick Start

### Prerequisites

Qt6 is already installed on your system. If you need to reinstall:
```bash
sudo apt-get install qt6-base-dev qt6-base-dev-tools libqt6openglwidgets6t64 libgl1-mesa-dev
```

### Environment Setup

The `MML_PROJECT_PATH` environment variable is already set in your `~/.bashrc`:
```bash
export MML_PROJECT_PATH=/home/zvanjak/Projects/MML_Visualizers/MML_Visualizers
```

For new shells, source it:
```bash
source ~/.bashrc
```

### Building

All visualizers are already built. To rebuild:
```bash
cd Qt
./build_all.sh
```

Or build individually:
```bash
cd Qt/MML_ParametricCurve3D_Visualizer
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

## Running Visualizers

### Option 1: Interactive Menu
```bash
cd Qt
./run_menu.sh
```

### Option 2: Run with All Test Data
```bash
cd Qt
./run_parametric_curve3d.sh    # Cycles through all 3D curve data
./run_particle_visualizer3d.sh  # Cycles through all particle data
./run_real_function.sh          # Cycles through all function data
./run_scalar_function2d.sh      # Cycles through all 2D scalar data
./run_vector_field3d.sh         # Cycles through all 3D vector field data
```

### Option 3: Run with Specific Data File
```bash
cd Qt
./qt_run_wrapper.sh ./MML_ParametricCurve3D_Visualizer/build/bin/MML_ParametricCurve3D_Visualizer_Qt $MML_PROJECT_PATH/data/ParametricCurve3D/example3_wpf_lorentz_system1.txt
```

**Note:** Use the `qt_run_wrapper.sh` to avoid snap library conflicts on Ubuntu.

## Available Visualizers

### 1. MML_ParametricCurve3D_Visualizer
Visualizes 3D parametric curves with OpenGL rendering.

**Features:**
- Interactive 3D camera controls (rotate, pan, zoom)
- Multiple curves with color coding
- Coordinate axes and reference grid

**Controls:**
- Left mouse: Rotate
- Right/Middle mouse: Pan
- Mouse wheel: Zoom

**Example:**
```bash
./MML_ParametricCurve3D_Visualizer/build/bin/MML_ParametricCurve3D_Visualizer_Qt \
    $MML_PROJECT_PATH/data/ParametricCurve3D/example3_wpf_lorentz_system1.txt
```

### 2. MML_ParticleVisualizer3D
Animates particle simulations in 3D space.

**Example:**
```bash
./MML_ParticleVisualizer3D/build/bin/MML_ParticleVisualizer3D_Qt \
    $MML_PROJECT_PATH/data/ParticleVisualizer3D/solar_system.txt
```

### 3. MML_RealFunctionVisualizer
Visualizes real-valued functions (1D plots).

**Example:**
```bash
./MML_RealFunctionVisualizer/build/bin/MML_RealFunctionVisualizer_Qt \
    $MML_PROJECT_PATH/data/RealFunction/real_func1.txt
```

### 4. MML_ScalarFunction2D_Visualizer
Visualizes 2D scalar fields with color mapping.

**Example:**
```bash
./MML_ScalarFunction2D_Visualizer/build/bin/MML_ScalarFunction2D_Visualizer_Qt \
    $MML_PROJECT_PATH/data/ScalarFunction2D/example_data.txt
```

### 5. MML_VectorField3D_Visualizer
Visualizes 3D vector fields with arrows.

**Example:**
```bash
./MML_VectorField3D_Visualizer/build/bin/MML_VectorField3D_Visualizer_Qt \
    $MML_PROJECT_PATH/data/VectorFieldVisualizer3D/example_data.txt
```

## Data Files

Test data files are located in:
```
$MML_PROJECT_PATH/data/
├── ParametricCurve3D/
├── ParticleVisualizer3D/
├── RealFunction/
├── ScalarFunction2D/
└── VectorFieldVisualizer3D/
```

## Troubleshooting

### Snap Library Conflicts

If you see errors like:
```
symbol lookup error: /snap/core20/current/lib/x86_64-linux-gnu/libpthread.so.0: 
undefined symbol: __libc_pthread_init
```

**Solution:** Always use the `qt_run_wrapper.sh` script:
```bash
./qt_run_wrapper.sh ./path/to/visualizer datafile.txt
```

### GTK Module Warnings

Harmless warnings like `Failed to load module "canberra-gtk-module"` can be ignored.

### Display Issues

If the application doesn't display:
1. Ensure `DISPLAY` environment variable is set: `echo $DISPLAY`
2. Check X11 authorization: `ls ~/.Xauthority`

## Build Status

✅ MML_ParametricCurve3D_Visualizer - Built and tested
✅ MML_ParticleVisualizer3D - Built and tested  
✅ MML_RealFunctionVisualizer - Built and tested
✅ MML_ScalarFunction2D_Visualizer - Built and tested
✅ MML_VectorField3D_Visualizer - Built and tested

## Development

All visualizers use:
- Qt 6.4.2
- OpenGL for 3D rendering
- CMake build system
- C++17 standard

To modify a visualizer, edit the source files and rebuild:
```bash
cd MML_<VisualizerName>
cd build
make -j$(nproc)
```
