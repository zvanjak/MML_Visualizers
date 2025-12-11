# Installing FLTK and Building the Visualizers

## Quick Install Guide for FLTK

### Option 1: Using vcpkg (Recommended)

1. **Install vcpkg** (if not already installed):
```powershell
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

2. **Install FLTK**:
```powershell
.\vcpkg install fltk:x64-windows
```

3. **Build the visualizers** (from each visualizer directory):
```powershell
cd D:\Projects\MML_Visualizers\FLTK\MML_ParametricCurve2D_Visualizer
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### Option 2: Download Pre-built FLTK

1. Download FLTK 1.3.8 from: https://www.fltk.org/software.php
2. Extract to `C:\FLTK`
3. Build FLTK:
```powershell
cd C:\FLTK
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
cmake --install . --prefix C:\FLTK-install
```

4. Build visualizers with FLTK path:
```powershell
cd D:\Projects\MML_Visualizers\FLTK\MML_ParametricCurve2D_Visualizer
mkdir build
cd build
cmake -DFLTK_DIR=C:/FLTK-install/lib/cmake ..
cmake --build . --config Release
```

## Running the Visualizers

Once built, run with test data:

### 1. Parametric Curve Visualizer
```powershell
cd D:\Projects\MML_Visualizers\FLTK\MML_ParametricCurve2D_Visualizer\build\bin\Release
.\MML_ParametricCurve2D_Visualizer.exe ..\..\..\..\WPF\MML_ParametricCurve2D_Visualizer\data\curve1.txt ..\..\..\..\WPF\MML_ParametricCurve2D_Visualizer\data\curve2.txt
```

### 2. Particle Visualizer
```powershell
cd D:\Projects\MML_Visualizers\FLTK\MML_ParticleVisualizer2D\build\bin\Release
.\MML_ParticleVisualizer2D.exe ..\..\..\..\WPF\MML_ParticleVisualizer2D\data\SimData.txt
```

### 3. Vector Field Visualizer
```powershell
cd D:\Projects\MML_Visualizers\FLTK\MML_VectorField2D_Visualizer\build\bin\Release
.\MML_VectorField2D_Visualizer.exe ..\..\..\..\WPF\MML_VectorField2D_Visualizer\data\example3_wpf_vector_field_2d.txt
```

## Alternative: Use WPF Visualizers (Windows Only)

Since you already have the WPF visualizers, you can build and run those:

```powershell
cd D:\Projects\MML_Visualizers\WPF

# Using Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" MML_Visualizers.sln /p:Configuration=Release /p:Platform="Any CPU"

# Run the applications
cd MML_ParametricCurve2D_Visualizer\bin\Release
.\MML_ParametricCurve2D_Visualizer.exe ..\..\data\curve1.txt

cd ..\..\MML_ParticleVisualizer2D\bin\Release
.\MML_ParticleVisualizer2D.exe ..\..\data\SimData.txt

cd ..\..\MML_VectorField2D_Visualizer\bin\Release
.\MML_VectorField2D_Visualizer.exe ..\..\data\example3_wpf_vector_field_2d.txt
```

## Build All Three at Once

From the FLTK directory (after FLTK is installed):

```powershell
cd D:\Projects\MML_Visualizers\FLTK
mkdir build_all
cd build_all

# If using vcpkg
cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..

# If FLTK is manually installed
cmake -DFLTK_DIR=C:/FLTK-install/lib/cmake ..

cmake --build . --config Release
```

Then run:
```powershell
.\bin\Release\MML_ParametricCurve2D_Visualizer.exe ..\WPF\MML_ParametricCurve2D_Visualizer\data\curve1.txt
.\bin\Release\MML_ParticleVisualizer2D.exe ..\WPF\MML_ParticleVisualizer2D\data\SimData.txt
.\bin\Release\MML_VectorField2D_Visualizer.exe ..\WPF\MML_VectorField2D_Visualizer\data\example3_wpf_vector_field_2d.txt
```
