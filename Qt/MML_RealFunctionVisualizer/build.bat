@echo off
echo Building MML_RealFunctionVisualizer (Qt)...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
if errorlevel 1 (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

REM Build
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

echo.
echo Build successful!
echo Executable: build\bin\Release\MML_RealFunctionVisualizer.exe
echo.
echo To deploy Qt DLLs, run:
echo C:\Dev\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe build\bin\Release\MML_RealFunctionVisualizer.exe
echo.

cd ..
