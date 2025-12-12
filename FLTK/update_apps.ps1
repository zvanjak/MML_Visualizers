# Update FLTK Visualizers for Windows
# This script copies FLTK visualizers to visualizers/win/FLTK/ if they are newer
# Similar to update_apps.sh for Linux

param(
    [switch]$Force,
    [switch]$Help
)

if ($Help) {
    Write-Host @"
Update FLTK Visualizers for Windows

Usage: .\update_apps.ps1 [-Force] [-Help]

Options:
  -Force        Force copy even if destination is newer
  -Help         Show this help message

This script:
1. Checks if source binaries are newer than deployed versions
2. Copies updated executables to visualizers/win/FLTK/ (only if newer)
3. Skips files that haven't changed

FLTK visualizers updated (with _FLTK suffix on Windows):
- MML_RealFunctionVisualizer_FLTK.exe
- MML_ParametricCurve2D_Visualizer_FLTK.exe
- MML_ParticleVisualizer2D_FLTK.exe
- MML_VectorField2D_Visualizer_FLTK.exe

Note: Build the visualizers first using CMake
"@
    exit 0
}

$ErrorActionPreference = "Stop"
$FltkFolder = $PSScriptRoot
$ProjectRoot = Split-Path $PSScriptRoot -Parent
$DeployDir = Join-Path $ProjectRoot "visualizers\win\FLTK"

# Define visualizers to update
$Visualizers = @(
    @{
        Name = "MML_RealFunctionVisualizer_FLTK"
        SourceDir = "MML_RealFunctionVisualizer"
    },
    @{
        Name = "MML_ParametricCurve2D_Visualizer_FLTK"
        SourceDir = "MML_ParametricCurve2D_Visualizer"
    },
    @{
        Name = "MML_ParticleVisualizer2D_FLTK"
        SourceDir = "MML_ParticleVisualizer2D"
    },
    @{
        Name = "MML_VectorField2D_Visualizer_FLTK"
        SourceDir = "MML_VectorField2D_Visualizer"
    }
)

Write-Host "=== Updating FLTK Visualizers for Windows ===" -ForegroundColor Cyan
Write-Host "Project Root: $ProjectRoot"
Write-Host "Deploy Directory: $DeployDir"
Write-Host "Mode: $(if ($Force) { 'Force copy' } else { 'Copy if newer' })"
Write-Host ""

# Create deploy directory if it doesn't exist
if (-not (Test-Path $DeployDir)) {
    Write-Host "Creating deploy directory..." -ForegroundColor Gray
    New-Item -ItemType Directory -Path $DeployDir | Out-Null
}

$updated = 0
$skipped = 0
$notFound = 0

foreach ($viz in $Visualizers) {
    Write-Host "Processing: $($viz.Name)" -ForegroundColor Yellow
    
    $sourceDir = Join-Path $FltkFolder $viz.SourceDir
    $releaseDir = Join-Path $sourceDir "build\bin\Release"
    $sourceFile = Join-Path $releaseDir "$($viz.Name).exe"
    $destFile = Join-Path $DeployDir "$($viz.Name).exe"
    
    # Check if source file exists
    if (-not (Test-Path $sourceFile)) {
        Write-Host "  ERROR: Not found at: $sourceFile" -ForegroundColor Red
        Write-Host "  Build the visualizer first!" -ForegroundColor Red
        $notFound++
        continue
    }
    
    # Get source file info
    $sourceInfo = Get-Item $sourceFile
    $sourceTime = $sourceInfo.LastWriteTime
    $sourceSize = [math]::Round($sourceInfo.Length / 1MB, 2)
    
    # Check if destination exists and compare timestamps
    $shouldCopy = $Force
    
    if (-not $Force) {
        if (-not (Test-Path $destFile)) {
            Write-Host "  -> Copying (first deployment)" -ForegroundColor Green
            $shouldCopy = $true
        } else {
            $destTime = (Get-Item $destFile).LastWriteTime
            
            if ($sourceTime -gt $destTime) {
                Write-Host "  -> Copying (built version is newer)" -ForegroundColor Green
                $shouldCopy = $true
            } else {
                Write-Host "  -> Skipped (deployed version is up to date)" -ForegroundColor Yellow
                $skipped++
            }
        }
    } else {
        Write-Host "  -> Force copying" -ForegroundColor Green
    }
    
    if ($shouldCopy) {
        Copy-Item -Force $sourceFile $destFile
        Write-Host "  COPIED: $($viz.Name) (~$sourceSize MB)" -ForegroundColor Green
        $updated++
    }
    
    Write-Host ""
}

# Summary
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Update Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Updated: $updated" -ForegroundColor $(if ($updated -gt 0) { "Green" } else { "Gray" })
Write-Host "Skipped (up to date): $skipped" -ForegroundColor $(if ($skipped -gt 0) { "Yellow" } else { "Gray" })
Write-Host "Not found: $notFound" -ForegroundColor $(if ($notFound -gt 0) { "Red" } else { "Gray" })
Write-Host "========================================" -ForegroundColor Cyan

if ($updated -gt 0) {
    Write-Host ""
    Write-Host "Deployed executables are in: $DeployDir" -ForegroundColor Green
    Write-Host ""
    Write-Host "Test the visualizers:" -ForegroundColor Cyan
    foreach ($viz in $Visualizers) {
        $exePath = Join-Path $DeployDir "$($viz.Name).exe"
        if (Test-Path $exePath) {
            Write-Host "  $exePath" -ForegroundColor Gray
        }
    }
}

exit 0
