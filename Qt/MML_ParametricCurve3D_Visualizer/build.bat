@echo off
REM Build script for MML_ParametricCurve3D_Visualizer

REM Find Qt installation
set QT_BASE=C:\Dev\Qt

REM Try to find Qt version automatically
for /d %%i in ("%QT_BASE%\6.*") do set QT_VERSION=%%~nxi

if not defined QT_VERSION (
    echo Error: Could not find Qt 6.x in %QT_BASE%
    echo Please adjust QT_BASE in this script
    exit /b 1
)

echo Found Qt version: %QT_VERSION%

REM Set Qt path
set QT_PATH=%QT_BASE%\%QT_VERSION%\msvc2019_64
if not exist "%QT_PATH%" (
    REM Try msvc2022
    set QT_PATH=%QT_BASE%\%QT_VERSION%\msvc2022_64
)

if not exist "%QT_PATH%" (
    echo Error: Could not find Qt MSVC compiler in %QT_BASE%\%QT_VERSION%
    echo Looking for: msvc2019_64 or msvc2022_64
    exit /b 1
)

echo Using Qt path: %QT_PATH%

REM Create and enter build directory
if not exist build mkdir build
cd build

REM Configure
echo Configuring...
cmake -DCMAKE_PREFIX_PATH=%QT_PATH% -G "Visual Studio 17 2022" ..
if errorlevel 1 (
    echo Configuration failed!
    cd ..
    exit /b 1
)

REM Build
echo Building...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

cd ..
echo.
echo Build successful!
echo Executable location: build\bin\Release\MML_ParametricCurve3D_Visualizer.exe
