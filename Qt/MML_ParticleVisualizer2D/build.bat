@echo off
REM Build script for MML_ParticleVisualizer2D (Qt version) on Windows

echo ========================================
echo Building MML_ParticleVisualizer2D (Qt)
echo ========================================

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

echo.
echo Configuring with CMake...
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
if errorlevel 1 (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

echo.
echo Building Release configuration...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

echo.
echo Deploying Qt dependencies...
cd Release
C:\Dev\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe MML_ParticleVisualizer2D.exe
if errorlevel 1 (
    echo Deployment failed!
    cd ..\..
    exit /b 1
)

cd ..\...

echo.
echo ========================================
echo Build completed successfully!
echo Executable: build\Release\MML_ParticleVisualizer2D.exe
echo ========================================
