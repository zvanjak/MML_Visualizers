@echo off
REM Quick run script for MML_ParticleVisualizer3D

SET EXE_PATH=build\bin\Release\MML_ParticleVisualizer3D.exe
SET DATA_PATH=..\..\WPF\MML_ParticleVisualizer3D\data

if not exist %EXE_PATH% (
    echo Error: Executable not found. Please run build.bat first.
    exit /b 1
)

echo Running MML_ParticleVisualizer3D...
echo.

REM Run with default test file
%EXE_PATH% %DATA_PATH%\SimData3D.txt

REM Alternative test files:
REM %EXE_PATH% %DATA_PATH%\SimDataGravity.txt
REM %EXE_PATH% %DATA_PATH%\collision_sim_3d_example2.txt
