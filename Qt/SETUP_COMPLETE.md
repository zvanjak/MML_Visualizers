# Qt Visualizers - Linux Setup Complete! ✓

## Summary

Successfully installed Qt6 and built all 5 Qt-based MML visualizers on Linux (Ubuntu).

## What Was Done

### 1. Installed Qt6 Development Environment
```bash
sudo apt-get install -y qt6-base-dev qt6-base-dev-tools libqt6openglwidgets6t64 libgl1-mesa-dev
```
- Qt version: 6.4.2
- All required OpenGL and GUI components installed

### 2. Built All Visualizers
All 5 visualizers successfully compiled:
- ✅ MML_ParametricCurve3D_Visualizer
- ✅ MML_ParticleVisualizer3D  
- ✅ MML_RealFunctionVisualizer
- ✅ MML_ScalarFunction2D_Visualizer
- ✅ MML_VectorField3D_Visualizer

### 3. Environment Configuration
- Set `MML_PROJECT_PATH` environment variable in `~/.bashrc`
- Created `qt_run_wrapper.sh` to avoid snap library conflicts
- Created symlinks for data directories

### 4. Helper Scripts Created
- `build_all.sh` - Rebuild all visualizers
- `test_all.sh` - Test all visualizers  
- `run_menu.sh` - Interactive launcher
- `qt_run_wrapper.sh` - Safe execution wrapper
- Updated all `run_*.sh` scripts to use correct paths

### 5. Documentation
- Created `README_LINUX.md` with complete usage instructions

## Quick Start

### Test All Visualizers
```bash
cd ~/Projects/MML_Visualizers/MML_Visualizers/Qt
./test_all.sh
```

### Run Interactive Menu
```bash
cd ~/Projects/MML_Visualizers/MML_Visualizers/Qt
./run_menu.sh
```

### Run Individual Visualizer
```bash
cd ~/Projects/MML_Visualizers/MML_Visualizers/Qt
./qt_run_wrapper.sh ./MML_ParametricCurve3D_Visualizer/build/bin/MML_ParametricCurve3D_Visualizer_Qt \
    $MML_PROJECT_PATH/data/ParametricCurve3D/example3_wpf_lorentz_system1.txt
```

## Technical Notes

### Snap Library Conflict
Ubuntu has a known issue where snap packages can cause library conflicts with Qt applications. The `qt_run_wrapper.sh` script solves this by:
- Using a clean environment
- Setting proper library paths
- Avoiding snap directories

### Data Directory Structure
```
$MML_PROJECT_PATH/data/
├── ParametricCurve3D/          # 3D curve data
├── ParticleVisualizer3D/       # Particle simulation data
├── RealFunction/               # Function plot data
├── ScalarFunction2D/ -> ../WPF/MML_ScalarFunction2D_Visualizer/data
└── VectorFieldVisualizer3D/ -> ../WPF/MML_VectorField3D_Visualizer/data
```

### Controls (3D Visualizers)
- **Left Mouse**: Rotate camera
- **Right/Middle Mouse**: Pan camera
- **Mouse Wheel**: Zoom in/out
- **Reset Button**: Return to default view

## Test Results

All visualizers tested and confirmed working:
```
Testing Parametric Curve 3D...     ✓ Working
Testing Particle Visualizer 3D...  ✓ Working
Testing Real Function Visualizer... ✓ Working
Testing Scalar Function 2D...      ✓ Working
Testing Vector Field 3D...         ✓ Working
```

## Next Steps

You can now:
1. Visualize your mathematical data with any of the 5 visualizers
2. Load multiple data files simultaneously
3. Use interactive 3D controls for exploration
4. Rebuild visualizers after modifications with `./build_all.sh`

## Files Modified/Created

### New Files
- `Qt/qt_run_wrapper.sh` - Execution wrapper for snap conflict avoidance
- `Qt/build_all.sh` - Build all visualizers script
- `Qt/test_all.sh` - Test all visualizers script
- `Qt/run_menu.sh` - Interactive launcher menu
- `Qt/README_LINUX.md` - Linux-specific documentation
- `Qt/SETUP_COMPLETE.md` - This file

### Modified Files
- `Qt/run_parametric_curve3d.sh` - Updated paths
- `Qt/run_particle_visualizer3d.sh` - Updated paths
- `Qt/run_real_function.sh` - Updated paths
- `Qt/run_scalar_function2d.sh` - Updated paths
- `Qt/run_vector_field3d.sh` - Updated paths
- `~/.bashrc` - Added MML_PROJECT_PATH

### Data Links Created
- `data/ScalarFunction2D` -> WPF data directory
- `data/VectorFieldVisualizer3D` -> WPF data directory

## Enjoy Your Visualizers! 🎨📊

All Qt visualizers are now fully operational on Linux!
