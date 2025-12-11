# QUICK START GUIDE

## For Windows Users (Getting Started in 5 Minutes)

### Step 1: Install FLTK
```powershell
# Using vcpkg (easiest method)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install fltk:x64-windows
```

### Step 2: Build the Application
```powershell
cd e:\Projects\MML_Visualizers\FLTK\MML_RealFunctionVisualizer

# If using vcpkg:
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
cd ..

# Or simply run:
build.bat
```

### Step 3: Test It!
```powershell
# Run with sample data
test.bat

# Or manually:
build\bin\Release\MML_RealFunctionVisualizer.exe ..\..\WPF\MML_RealFunctionVisualizer\data\real_func1.txt
```

## What You Get

✅ **Cross-platform** MML function visualizer  
✅ **Full feature parity** with WPF version  
✅ **Beautiful graphs** with automatic scaling  
✅ **Multi-function support** with color-coded legend  
✅ **Easy to use** - drag & drop files or use load button  

## Sample Commands

```bash
# View single function
MML_RealFunctionVisualizer data\real_func1.txt

# View multiple functions
MML_RealFunctionVisualizer data\real_func1.txt data\real_func2.txt

# View multi-function dataset
MML_RealFunctionVisualizer data\multi_real_func.txt

# Interactive mode (use Load button)
MML_RealFunctionVisualizer
```

## Supported Data Files

All MML data files from `../../WPF/MML_RealFunctionVisualizer/data/`:
- ✅ real_func1.txt
- ✅ real_func2.txt  
- ✅ multi_real_func.txt
- ✅ projectile_y(x).txt
- ✅ double_pendulum_multi_real_func.txt
- ✅ And more...

## UI Controls

- **Load File Button**: Open file dialog to load MML data
- **Clear Button**: Clear all loaded functions
- **Graph Area**: Displays function plots with grid and axes
- **Legend Area**: Shows function names with colors

## Need Help?

- See `README.md` for full documentation
- See `INSTALL_FLTK.md` for FLTK installation details
- See `PROJECT_OVERVIEW.md` for technical details

## Common Issues

**Problem**: CMake can't find FLTK  
**Solution**: Use vcpkg or set FLTK_DIR environment variable

**Problem**: Build fails with C++17 errors  
**Solution**: Update Visual Studio to 2017 or later

**Problem**: File won't load  
**Solution**: Check file format matches MML specification

Enjoy your cross-platform MML visualizer! 🎉
