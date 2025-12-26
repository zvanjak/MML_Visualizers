# MML Real Function Visualizer (FLTK)

A cross-platform MML (Mathematical Modeling Library) Real Function Visualizer built with FLTK (Fast Light Toolkit). This version closely matches the functionality of the WPF (Windows) version.

## Features

### Graph Visualization
- Load and visualize real function data from MML text files
- Support for single and multi-function datasets
- **"Nice numbers" axis tick algorithm** - ticks placed at visually pleasing intervals (1, 2, 2.5, 5, 10)
- **Smart axis positioning** - axes drawn at y=0/x=0 when in range, otherwise at edges
- Automatic scaling with optional preserved aspect ratio
- Grid overlay with configurable visibility
- **Scientific notation** for very large or very small numbers

### User Interface
- **WPF-style sidebar layout** (230px on right side)
- **Editable graph title** - updates window title
- **Legend with visibility checkboxes** - toggle individual functions on/off
- **Graph settings section**:
  - Show/Hide Grid
  - Show/Hide Axis Labels
  - Preserve Aspect Ratio
- Load File and Clear All buttons

### Window Layout
- Default window size: 1200×850 pixels
- Graph area with proper margins
- GTK+ visual scheme for modern appearance

## Supported File Formats

### REAL_FUNCTION
Single function with x-y pairs:
```
REAL_FUNCTION
Title of the function
x1: <min_x>
x2: <max_x>
NumPoints: <num_points>
<x1> <y1>
<x2> <y2>
...
```

### MULTI_REAL_FUNCTION
Multiple functions with shared x-coordinates and legend:
```
MULTI_REAL_FUNCTION
Title
<dimension>
<function1_name>
<function2_name>
...
x1: <min_x>
x2: <max_x>
NumPoints: <num_points>
<x> <y1> <y2> ...
```

### MULTI_REAL_FUNCTION_VARIABLE_SPACED
Multiple functions with variable x-spacing (common for time-series data):
```
MULTI_REAL_FUNCTION_VARIABLE_SPACED
Title
<dimension>
<num_points>
<start_time>
<end_time>
<x1> <y1_1> <y1_2> ...
<x2> <y2_1> <y2_2> ...
...
```

## Building

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler
- FLTK 1.3.x or higher

### Linux

1. Install FLTK (see [LINUX_INSTALL.md](../LINUX_INSTALL.md) for details):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libfltk1.3-dev libfltk-gl1.3 libfltk-images1.3
   
   # Fedora
   sudo dnf install fltk-devel
   
   # Arch Linux
   sudo pacman -S fltk
   ```

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

### Windows

1. Install FLTK (using vcpkg):
   ```powershell
   vcpkg install fltk:x64-windows
   ```

2. Build the project:
   ```powershell
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

### macOS

1. Install FLTK:
   ```bash
   brew install fltk
   ```

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Usage

### Command Line
```bash
# Run with no files (load later via GUI)
./build/bin/MML_RealFunctionVisualizer_FLTK

# Load a single file
./build/bin/MML_RealFunctionVisualizer_FLTK path/to/data.txt

# Load multiple files
./build/bin/MML_RealFunctionVisualizer_FLTK file1.txt file2.txt
```

### Sample Data Files
Test data files are available in the WPF version's data directory:
```bash
./build/bin/MML_RealFunctionVisualizer_FLTK \
    ../../WPF/MML_RealFunctionVisualizer/data/real_func1.txt
```

Available test files:
- `real_func1.txt`, `real_func2.txt` - Single function examples
- `multi_real_func.txt` - Multi-function example
- `projectile_*.txt` - Projectile motion simulations
- `double_pendulum_*.txt` - Double pendulum simulations
- `spherical_pendulum_*.txt` - Spherical pendulum simulations
- `example3_wpf_lorentz.txt` - Lorenz attractor data

### Interactive Controls
- **Legend checkboxes**: Click to show/hide individual functions
- **Graph Title**: Type a new title and press Enter
- **Settings checkboxes**: Toggle grid, labels, and aspect ratio

## Architecture

### Source Files
- `main.cpp` - Application entry point and MainWindow class
- `GraphWidget.h/cpp` - Main graph rendering widget
- `LegendWidget.h/cpp` - Legend with visibility checkboxes
- `AxisTickCalculator.h/cpp` - Nice numbers algorithm for axis ticks
- `MMLData.h` - Data structures (LoadedFunction, CoordSystemParams)
- `MMLFileParser.h/cpp` - File parsing for MML formats

### Color Palette
Functions are assigned colors in order:
1. Black, 2. Blue, 3. Red, 4. Green, 5. Orange
6. Purple, 7. Brown, 8. Cyan, 9. Magenta, 10. Gray, 11. Yellow

## Version History

### v2.0 (Current)
- Added WPF-style sidebar layout
- Implemented "nice numbers" axis tick algorithm
- Added legend with visibility checkboxes
- Added preserve aspect ratio option
- Added support for MULTI_REAL_FUNCTION_VARIABLE_SPACED format
- Improved coordinate system rendering (axes at 0 or edges)
- Added scientific notation for extreme values
- Updated window to 1200×850 layout

### v1.0
- Initial FLTK port
- Basic graph rendering
- Simple legend display
- File loading support

## License

This project is part of the MML Visualizers collection.

## See Also

- [WPF Version](../../WPF/MML_RealFunctionVisualizer/) - Original Windows version
- [FLTK Library](https://www.fltk.org/) - Cross-platform GUI toolkit
