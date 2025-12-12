@echo off
REM Quick test script for MML_VectorField3D_Visualizer

set EXE=build\bin\Release\MML_VectorField3D_Visualizer.exe
set DATA=..\..\WPF\MML_VectorField3D_Visualizer\data\vector_field.txt

if not exist "%EXE%" (
    echo ERROR: Executable not found: %EXE%
    echo Please build the project first.
    pause
    exit /b 1
)

if not exist "%DATA%" (
    echo ERROR: Data file not found: %DATA%
    pause
    exit /b 1
)

echo Running MML_VectorField3D_Visualizer...
echo Data: %DATA%
echo.

"%EXE%" "%DATA%"
