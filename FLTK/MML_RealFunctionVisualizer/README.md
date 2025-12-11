# MML Real Function Visualizer (FLTK)

A cross-platform MML (Mathematical Modeling Library) Real Function Visualizer built with FLTK (Fast Light Toolkit).

## Features

- Load and visualize real function data from MML text files
- Support for both single and multi-function datasets
- Interactive legend showing function names and colors
- Automatic scaling and coordinate system rendering
- Cross-platform compatibility (Windows, Linux, macOS)

## Supported File Formats

### REAL_FUNCTION
Single function with equally or variable spaced points:
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
Multiple functions with shared x-coordinates:
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
<x> <y1> <y2> ...
...
```

## Building

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler
- FLTK 1.3.x or higher

### Windows

1. Install FLTK (you can use vcpkg or build from source)
   ```
   vcpkg install fltk
   ```

2. Build the project:
   ```powershell
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

### Linux

1. Install FLTK:
   ```bash
   sudo apt-get install libfltk1.3-dev  # Ubuntu/Debian
   # or
   sudo dnf install fltk-devel          # Fedora
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

### macOS

1. Install FLTK:
   ```bash
   brew install fltk
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

### Run with command-line arguments:
```bash
./MML_RealFunctionVisualizer path/to/data/file.txt [path/to/another/file.txt ...]
```

### Run and load files interactively:
```bash
./MML_RealFunctionVisualizer
```
Then use the "Load File" button to open MML data files.

## Example Data

Sample data files are available in the `../../WPF/MML_RealFunctionVisualizer/data/` directory:
- `real_func1.txt` - Single function example
- `multi_real_func.txt` - Multiple functions example
- `projectile_y(x).txt` - Projectile motion
- And more...

## Architecture

The application is structured as follows:

- **MMLData.h/cpp**: Data structures for functions and coordinate system parameters
- **MMLFileParser.h/cpp**: Parser for MML text file formats
- **GraphWidget.h/cpp**: Custom FLTK widget for rendering function graphs
- **LegendWidget.h/cpp**: Custom FLTK widget for displaying the legend
- **main.cpp**: Main application window and event handling

## Features Comparison with WPF Version

| Feature | FLTK Version | WPF Version |
|---------|--------------|-------------|
| Load REAL_FUNCTION | ✅ | ✅ |
| Load MULTI_REAL_FUNCTION | ✅ | ✅ |
| Interactive legend | ✅ | ✅ |
| Automatic scaling | ✅ | ✅ |
| Grid rendering | ✅ | ✅ |
| Multiple colors | ✅ | ✅ |
| Cross-platform | ✅ | ❌ |

## License

This project is part of the MML_Visualizers suite.
