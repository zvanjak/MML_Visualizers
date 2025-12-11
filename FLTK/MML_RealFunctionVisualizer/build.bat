@echo off
REM Build script for MML_RealFunctionVisualizer on Windows

echo Building MML_RealFunctionVisualizer...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022"

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

REM Build the project
echo Building...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    cd ..
    pause
    exit /b 1
)

cd ..

echo Build completed successfully!
echo Executable is in: build\bin\Release\MML_RealFunctionVisualizer.exe

pause
