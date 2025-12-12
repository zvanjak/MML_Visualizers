# Qt Visualizers for Windows

This directory contains self-contained Qt visualizers with all necessary DLLs for Windows deployment.

## Contents

### MML_ParametricCurve3D_Visualizer
- **Executable**: `MML_ParametricCurve3D_Visualizer.exe` (111 KB)
- **Purpose**: 3D parametric curve visualization with OpenGL
- **Features**: Interactive 3D camera controls, multiple curves, orbit/pan/zoom
- **Data Format**: PARAMETRIC_CURVE_CARTESIAN_3D

### MML_RealFunctionVisualizer
- **Executable**: `MML_RealFunctionVisualizer.exe` (101 KB)
- **Purpose**: 2D real function visualization with OpenGL
- **Features**: Multi-function support, pan/zoom, grid, axes
- **Data Format**: REAL_FUNCTION

## Deployment

Each visualizer directory contains:
- **Main executable** (*.exe)
- **Qt DLLs** (Qt6Core, Qt6Gui, Qt6Widgets, Qt6OpenGL, Qt6OpenGLWidgets, Qt6Network, Qt6Svg)
- **System DLLs** (D3Dcompiler_47.dll, opengl32sw.dll, icuuc.dll)
- **Plugin directories**:
  - `platforms/` - Platform integration (qwindows.dll)
  - `styles/` - UI styles (qmodernwindowsstyle.dll)
  - `iconengines/` - Icon rendering (qsvgicon.dll)
  - `imageformats/` - Image loading (qgif, qico, qjpeg, qsvg)
  - `generic/`, `networkinformation/`, `tls/` - Additional plugins
  - `translations/` - Qt translations (31 languages)

**Total Size per Visualizer**: ~56 MB (with all Qt dependencies)

## Usage

### Command Line

```bash
# 3D Parametric Curves
.\MML_ParametricCurve3D_Visualizer\MML_ParametricCurve3D_Visualizer.exe <curve-file1> <curve-file2> ...

# 2D Real Functions
.\MML_RealFunctionVisualizer\MML_RealFunctionVisualizer.exe <function-file1> <function-file2> ...
```

### Examples

```bash
# 3D: Lorenz attractor
.\MML_ParametricCurve3D_Visualizer\MML_ParametricCurve3D_Visualizer.exe ^
  ..\..\WPF\MML_ParametricCurve3D_Visualizer\data\example3_wpf_lorentz_system.txt

# 3D: 4-body trajectories
.\MML_ParametricCurve3D_Visualizer\MML_ParametricCurve3D_Visualizer.exe ^
  ..\..\WPF\MML_ParametricCurve3D_Visualizer\data\body0.txt ^
  ..\..\WPF\MML_ParametricCurve3D_Visualizer\data\body1.txt ^
  ..\..\WPF\MML_ParametricCurve3D_Visualizer\data\body2.txt ^
  ..\..\WPF\MML_ParametricCurve3D_Visualizer\data\body3.txt

# 2D: Real functions
.\MML_RealFunctionVisualizer\MML_RealFunctionVisualizer.exe ^
  ..\..\WPF\MML_RealFunctionVisualizer\data\real_func1.txt ^
  ..\..\WPF\MML_RealFunctionVisualizer\data\real_func2.txt
```

## Requirements

- **Windows 10/11** (64-bit)
- **OpenGL support** (typically available with graphics drivers)
- **No Qt installation required** - all DLLs included

## Controls

### MML_ParametricCurve3D_Visualizer (3D)
- **Left Mouse + Drag**: Rotate (orbit camera)
- **Right/Middle Mouse + Drag**: Pan view
- **Mouse Wheel**: Zoom in/out
- **Reset View Button**: Restore default camera

### MML_RealFunctionVisualizer (2D)
- **Left/Right Mouse + Drag**: Pan view
- **Mouse Wheel**: Zoom in/out
- **Load Function Button**: Add more functions
- **Reset View Button**: Auto-fit all functions

## Known Issues

- HTML color format warnings in console (harmless, Qt QTextHtmlParser quirk)
- These are cosmetic and don't affect functionality

## Build Information

- **Qt Version**: 6.10.0
- **Compiler**: MSVC 19.44.35221.0 (Visual Studio 2022 Enterprise)
- **Build Type**: Release
- **C++ Standard**: C++17
- **Deployment Tool**: windeployqt

## Architecture

Both visualizers follow the same pattern:
- **MainWindow**: Qt Widgets UI with controls and info display
- **GLWidget**: QOpenGLWidget for OpenGL rendering
- **MMLData**: Data structures (curves, functions, colors)
- **MMLFileParser**: Data file parsing
- **CMake build system** with Qt6 + OpenGL

## Directory Structure

```
Qt/
├── MML_ParametricCurve3D_Visualizer/
│   ├── MML_ParametricCurve3D_Visualizer.exe
│   ├── Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll
│   ├── Qt6OpenGL.dll, Qt6OpenGLWidgets.dll
│   ├── D3Dcompiler_47.dll, opengl32sw.dll
│   └── platforms/, styles/, iconengines/, etc.
└── MML_RealFunctionVisualizer/
    ├── MML_RealFunctionVisualizer.exe
    ├── Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll
    ├── Qt6OpenGL.dll, Qt6OpenGLWidgets.dll
    ├── D3Dcompiler_47.dll, opengl32sw.dll
    └── platforms/, styles/, iconengines/, etc.
```

## Distribution

These visualizers are fully self-contained and can be:
- Run directly from this directory
- Copied to other Windows machines
- Distributed as standalone applications
- No Qt installation needed on target machine

## Performance

- **Hardware Accelerated**: Uses OpenGL for smooth rendering
- **Responsive**: Interactive pan/zoom at 60+ FPS
- **Scalable**: Handles thousands of points efficiently
- **Multi-threaded**: Qt event loop + OpenGL rendering

## See Also

- Source code: `../../Qt/MML_ParametricCurve3D_Visualizer/`
- Source code: `../../Qt/MML_RealFunctionVisualizer/`
- FLTK versions: `../../visualizers/win/FLTK/` (coming soon)
- WPF versions: `../../WPF/`

## Version History

- **v1.0** (2025-12-12): Initial deployment
  - MML_ParametricCurve3D_Visualizer: 3D curve visualization
  - MML_RealFunctionVisualizer: 2D function visualization
  - Qt 6.10.0 with OpenGL acceleration
  - Full Qt DLL deployment via windeployqt
