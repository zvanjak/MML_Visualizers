@echo off
REM Build script for MML_ParticleVisualizer3D

SET QT_PATH=C:\Dev\Qt\6.10.0\msvc2022_64
SET BUILD_DIR=build

echo Building MML_ParticleVisualizer3D...

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Configure with CMake
cmake -DCMAKE_PREFIX_PATH=%QT_PATH% -G "Visual Studio 17 2022" ..
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

REM Build Release
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    cd ..
    exit /b 1
)

cd ..
echo Build successful!
echo Executable: %BUILD_DIR%\bin\Release\MML_ParticleVisualizer3D.exe
