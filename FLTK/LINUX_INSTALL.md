# FLTK Visualizers - Linux Installation Guide

This guide covers installing FLTK 1.3 on Linux and building the MML FLTK visualizers.

## Prerequisites

- Linux distribution (Ubuntu/Debian-based recommended)
- CMake 3.10 or later
- C++ compiler with C++17 support (g++ or clang++)
- X11 development libraries (usually pre-installed)

## Installing FLTK 1.3 on Ubuntu/Debian

### Quick Install

Install FLTK 1.3 runtime and development libraries:

```bash
sudo apt update
sudo apt install -y libfltk1.3t64 libfltk1.3-dev libfltk-gl1.3t64 libfltk-images1.3t64
```

This installs:
- `libfltk1.3t64` - Main FLTK runtime library (version 1.3.11)
- `libfltk1.3-dev` - Development headers and CMake config files
- `libfltk-gl1.3t64` - OpenGL rendering support
- `libfltk-images1.3t64` - Image loading support (PNG, JPEG, etc.)

### Verify Installation

Check that FLTK is properly installed:

```bash
# Check installed version
dpkg -l | grep libfltk1.3

# Verify library location
ldconfig -p | grep fltk

# Check for fltk-config (optional tool)
which fltk-config
```

Expected output:
```
libfltk1.3t64:amd64           1.3.11-3
libfltk.so.1.3 => /usr/lib/x86_64-linux-gnu/libfltk.so.1.3
```

## Building the Visualizers

### 1. Navigate to FLTK Directory

```bash
cd /path/to/MML_Visualizers/FLTK
```

### 2. Create Build Directory

```bash
mkdir -p build
cd build
```

### 3. Configure with CMake

```bash
cmake ..
```

CMake will automatically find FLTK using `find_package(FLTK REQUIRED)`.

### 4. Build All Visualizers

```bash
make -j$(nproc)
```

This builds all 4 FLTK visualizers in parallel using all CPU cores.

### 5. Verify Build

Check that binaries were created:

```bash
ls -lh bin/
```

Expected output:
```
MML_RealFunctionVisualizer_FLTK
MML_ParametricCurve2D_Visualizer_FLTK
MML_ParticleVisualizer2D_FLTK
MML_VectorField2D_Visualizer_FLTK
```

## Deploying Visualizers

Deploy built binaries to the deployment directory:

```bash
cd /path/to/MML_Visualizers/FLTK
./update_apps.sh
```

This script copies binaries to `../visualizers/linux/FLTK/` only if they're newer than existing versions.

## Running Visualizers

### From Deployment Directory

```bash
cd /path/to/MML_Visualizers/visualizers/linux/FLTK

# Run individual visualizers
./run_real_function.sh
./run_parametric_curve_2d.sh
./run_particle_2d.sh
./run_vector_field_2d.sh
```

### Direct Execution

```bash
cd /path/to/MML_Visualizers/FLTK/build/bin

# Run with data file
./MML_RealFunctionVisualizer_FLTK ../../data/RealFunction/Basic/sin.txt
```

## Troubleshooting

### Issue: "libfltk.so.1.3: cannot open shared object file"

**Cause**: FLTK runtime library not installed.

**Solution**:
```bash
sudo apt install libfltk1.3t64
```

Verify the library is now found:
```bash
ldd ./MML_RealFunctionVisualizer_FLTK | grep fltk
```

Expected output:
```
libfltk.so.1.3 => /usr/lib/x86_64-linux-gnu/libfltk.so.1.3
```

### Issue: CMake can't find FLTK

**Cause**: Development files not installed.

**Solution**:
```bash
sudo apt install libfltk1.3-dev
```

Then rebuild:
```bash
cd build
rm -rf *
cmake ..
make -j$(nproc)
```

### Issue: Visualizer window doesn't appear

**Cause**: X11 display not available (common in SSH sessions).

**Solution**: Either run locally or enable X11 forwarding:
```bash
ssh -X user@host
```

### Issue: Data files not found

**Cause**: Running from wrong directory or incorrect data paths.

**Solution**: Use the run scripts which handle paths correctly:
```bash
cd /path/to/MML_Visualizers/visualizers/linux/FLTK
./run_real_function.sh
```

Or specify absolute path to data file:
```bash
./MML_RealFunctionVisualizer_FLTK /absolute/path/to/data.txt
```

## Other Linux Distributions

### Fedora/RHEL/CentOS

```bash
sudo dnf install fltk fltk-devel
```

### Arch Linux

```bash
sudo pacman -S fltk
```

### From Source (Advanced)

If FLTK 1.3 is not available in your distribution:

```bash
# Download FLTK 1.3.x
wget https://www.fltk.org/pub/fltk/1.3.11/fltk-1.3.11-source.tar.gz
tar xzf fltk-1.3.11-source.tar.gz
cd fltk-1.3.11

# Build and install
./configure --enable-shared
make -j$(nproc)
sudo make install
sudo ldconfig
```

## Performance Notes

- FLTK is very lightweight (~600KB runtime library)
- Minimal dependencies (just X11 on Linux)
- Excellent for 2D scientific visualization
- Fast startup and low memory footprint
- Native widget rendering (no GPU required for UI)

## CMake Integration Details

The FLTK visualizers use standard CMake `find_package`:

```cmake
find_package(FLTK REQUIRED)
target_link_libraries(my_visualizer ${FLTK_LIBRARIES})
target_include_directories(my_visualizer PRIVATE ${FLTK_INCLUDE_DIR})
```

CMake searches for FLTK in:
1. `/usr/lib/fltk/` (Ubuntu/Debian)
2. `/usr/local/lib/fltk/` (from source install)
3. Environment variable `FLTK_DIR`

## Additional Resources

- FLTK Official Website: https://www.fltk.org/
- FLTK 1.3 Documentation: https://www.fltk.org/doc-1.3/
- Ubuntu Package Info: https://packages.ubuntu.com/search?keywords=libfltk1.3

## Version Information

- **FLTK Version Required**: 1.3.x (tested with 1.3.11)
- **CMake Minimum**: 3.10
- **C++ Standard**: C++17
- **Supported Distributions**: Ubuntu 24.04+, Debian 12+, Fedora 38+

---

**Last Updated**: December 26, 2025  
**Status**: ✅ Tested and verified on Ubuntu 24.04 with FLTK 1.3.11
