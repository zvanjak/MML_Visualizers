@echo off
REM Build script for Windows

echo Building MML_ParametricCurve2D_Visualizer...

if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

mkdir build
cd build

cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
if errorlevel 1 (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

echo.
echo Build successful!
echo.
echo Executable location: build\bin\Release\MML_ParametricCurve2D_Visualizer.exe
echo.

cd ..
