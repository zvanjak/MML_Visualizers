@echo off
REM Quick test script for MML_ScalarFunction2D_Visualizer

set EXE=build\bin\Release\MML_ScalarFunction2D_Visualizer.exe
set DATA1=..\..\WPF\MML_ScalarFunction2D_Visualizer\data\example3_wpf_surface1.txt
set DATA2=..\..\WPF\MML_ScalarFunction2D_Visualizer\data\example3_wpf_surface2.txt

if not exist "%EXE%" (
    echo ERROR: Executable not found: %EXE%
    echo Please build the project first.
    pause
    exit /b 1
)

if not exist "%DATA1%" (
    echo ERROR: Data file not found: %DATA1%
    pause
    exit /b 1
)

echo Running MML_ScalarFunction2D_Visualizer...
echo Data: %DATA1%
echo.

"%EXE%" "%DATA1%"
