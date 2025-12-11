@echo off
REM Run all test cases

set EXE=build\bin\Release\MML_RealFunctionVisualizer.exe
set DATA=..\..\WPF\MML_RealFunctionVisualizer\data

if not exist %EXE% (
    echo Error: Executable not found. Please build the project first.
    echo Run: cmake --build build --config Release
    pause
    exit /b 1
)

echo ============================================
echo MML Real Function Visualizer - Test Suite
echo ============================================
echo.

echo Test 1: Single Function (real_func1.txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\real_func1.txt

timeout /t 2 >nul

echo.
echo Test 2: Single Function (real_func2.txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\real_func2.txt

timeout /t 2 >nul

echo.
echo Test 3: Multi-Function (multi_real_func.txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\multi_real_func.txt

timeout /t 2 >nul

echo.
echo Test 4: Multiple Files (real_func1.txt + real_func2.txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\real_func1.txt %DATA%\real_func2.txt

timeout /t 2 >nul

echo.
echo Test 5: Projectile Motion (projectile_y(x).txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\projectile_y(x).txt

timeout /t 2 >nul

echo.
echo Test 6: Double Pendulum Multi (double_pendulum_multi_real_func.txt)
echo Press any key to launch...
pause >nul
start "" %EXE% %DATA%\double_pendulum_multi_real_func.txt

echo.
echo ============================================
echo All tests launched!
echo Close windows manually when done.
echo ============================================
pause
