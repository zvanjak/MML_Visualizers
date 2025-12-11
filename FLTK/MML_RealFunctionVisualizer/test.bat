@echo off
REM Test script - runs the visualizer with sample data

set DATA_DIR=..\..\WPF\MML_RealFunctionVisualizer\data

if not exist build\bin\Release\MML_RealFunctionVisualizer.exe (
    echo Error: Executable not found. Please build the project first.
    echo Run build.bat to build the project.
    pause
    exit /b 1
)

echo Testing with sample data...
echo.

REM Test with single function
echo Test 1: Single function (real_func1.txt)
build\bin\Release\MML_RealFunctionVisualizer.exe %DATA_DIR%\real_func1.txt

REM Uncomment to test with multi-function
REM echo Test 2: Multi-function (multi_real_func.txt)
REM build\bin\Release\MML_RealFunctionVisualizer.exe %DATA_DIR%\multi_real_func.txt

REM Uncomment to test with multiple files
REM echo Test 3: Multiple files
REM build\bin\Release\MML_RealFunctionVisualizer.exe %DATA_DIR%\real_func1.txt %DATA_DIR%\real_func2.txt
