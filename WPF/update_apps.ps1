# Update WPF Visualizers for Windows
# This script copies WPF visualizers to visualizers/win/WPF/ if they are newer
# Similar to Qt update_apps.ps1

param(
    [switch]$Force,
    [switch]$Help
)

if ($Help) {
    Write-Host @"
Update WPF Visualizers for Windows

Usage: .\update_apps.ps1 [-Force] [-Help]

Options:
  -Force        Force copy even if destination is newer
  -Help         Show this help message

This script:
1. Checks if source binaries are newer than deployed versions
2. Copies updated files to visualizers/win/WPF/ (only if newer)
3. Skips files that haven't changed

WPF visualizers updated:
- MML_RealFunctionVisualizer
- MML_ParametricCurve2D_Visualizer
- MML_ParametricCurve3D_Visualizer
- MML_ParticleVisualizer2D
- MML_ParticleVisualizer3D
- MML_VectorField2D_Visualizer
- MML_VectorField3D_Visualizer
- MML_ScalarFunction2D_Visualizer

Note: Build WPF apps in Visual Studio (Release configuration) before running this script
"@
    exit 0
}

$ErrorActionPreference = "Stop"
$WpfFolder = $PSScriptRoot
$ProjectRoot = Split-Path $PSScriptRoot -Parent

# Define visualizers to update
# Maps WPF project names to their deployment folder names
$Visualizers = @(
    @{
        Name = "MML_RealFunctionVisualizer"
        ProjectName = "MML_RealFunctionVisualizer"
        DestFolder = "real_function_visualizer"
    },
    @{
        Name = "MML_ParametricCurve2D_Visualizer"
        ProjectName = "MML_ParametricCurve2D_Visualizer"
        DestFolder = "parametric_curve_2d_visualizer"
    },
    @{
        Name = "MML_ParametricCurve3D_Visualizer"
        ProjectName = "MML_ParametricCurve3D_Visualizer"
        DestFolder = "parametric_curve_3d_visualizer"
    },
    @{
        Name = "MML_ParticleVisualizer2D"
        ProjectName = "MML_ParticleVisualizer2D"
        DestFolder = "particle_2d_visualizer"
    },
    @{
        Name = "MML_ParticleVisualizer3D"
        ProjectName = "MML_ParticleVisualizer3D"
        DestFolder = "particle_3d_visualizer"
    },
    @{
        Name = "MML_VectorField2D_Visualizer"
        ProjectName = "MML_VectorField2D_Visualizer"
        DestFolder = "vector_field_2d_visualizer"
    },
    @{
        Name = "MML_VectorField3D_Visualizer"
        ProjectName = "MML_VectorField3D_Visualizer"
        DestFolder = "vector_field_3d_visualizer"
    },
    @{
        Name = "MML_ScalarFunction2D_Visualizer"
        ProjectName = "MML_ScalarFunction2D_Visualizer"
        DestFolder = "scalar_function_2d_visualizer"
    }
)

Write-Host "=== Updating WPF Visualizers for Windows ===" -ForegroundColor Cyan
Write-Host "Project Root: $ProjectRoot"
Write-Host "Mode: $(if ($Force) { 'Force copy' } else { 'Copy if newer' })"
Write-Host ""

$successCount = 0
$skippedCount = 0

foreach ($viz in $Visualizers) {
    Write-Host "Processing: $($viz.Name)" -ForegroundColor Yellow
    
    $projectDir = Join-Path $WpfFolder $viz.ProjectName
    $sourceDir = Join-Path $projectDir "bin\Release\net8.0-windows"
    $destDir = Join-Path $ProjectRoot "visualizers\win\WPF\$($viz.DestFolder)"
    
    # Check if source directory exists
    if (-not (Test-Path $sourceDir)) {
        Write-Host "  WARNING: Source directory not found: $sourceDir" -ForegroundColor Yellow
        Write-Host "  Build the visualizer in Visual Studio (Release configuration) first!" -ForegroundColor Yellow
        $skippedCount++
        continue
    }
    
    # Check if exe exists
    $exeName = "$($viz.ProjectName).exe"
    $exePath = Join-Path $sourceDir $exeName
    if (-not (Test-Path $exePath)) {
        Write-Host "  WARNING: Executable not found: $exePath" -ForegroundColor Yellow
        $skippedCount++
        continue
    }
    
    # Create destination directory if it doesn't exist
    if (-not (Test-Path $destDir)) {
        Write-Host "  Creating destination directory..." -ForegroundColor Gray
        New-Item -ItemType Directory -Path $destDir | Out-Null
    }
    
    # Get all files from source (only specific extensions for WPF apps)
    $fileExtensions = @("*.exe", "*.dll", "*.json", "*.config", "*.pdb")
    $sourceFiles = @()
    foreach ($ext in $fileExtensions) {
        $sourceFiles += Get-ChildItem -Path $sourceDir -Filter $ext -File -ErrorAction SilentlyContinue
    }
    
    if ($sourceFiles.Count -eq 0) {
        Write-Host "  WARNING: No files found to copy" -ForegroundColor Yellow
        $skippedCount++
        continue
    }
    
    $copiedCount = 0
    $skippedFileCount = 0
    $updatedCount = 0
    
    Write-Host "  Checking files..." -ForegroundColor Gray
    
    foreach ($sourceFile in $sourceFiles) {
        $destFile = Join-Path $destDir $sourceFile.Name
        
        $shouldCopy = $Force
        
        if (-not $Force) {
            if (-not (Test-Path $destFile)) {
                $shouldCopy = $true
                $copiedCount++
            } elseif ($sourceFile.LastWriteTime -gt (Get-Item $destFile).LastWriteTime) {
                $shouldCopy = $true
                $updatedCount++
            } else {
                $skippedFileCount++
            }
        } else {
            $copiedCount++
        }
        
        if ($shouldCopy) {
            Copy-Item -Force $sourceFile.FullName $destFile
        }
    }
    
    # Count total files and size
    $totalFiles = $sourceFiles.Count
    $totalSize = ($sourceFiles | Measure-Object -Property Length -Sum).Sum / 1MB
    
    Write-Host "  Results:" -ForegroundColor Green
    if ($Force) {
        Write-Host "    - Copied: $copiedCount files (~$([math]::Round($totalSize, 2)) MB)" -ForegroundColor Green
    } else {
        if ($copiedCount -gt 0) {
            Write-Host "    - New files: $copiedCount" -ForegroundColor Green
        }
        if ($updatedCount -gt 0) {
            Write-Host "    - Updated: $updatedCount" -ForegroundColor Green
        }
        if ($skippedFileCount -gt 0) {
            Write-Host "    - Unchanged: $skippedFileCount" -ForegroundColor Gray
        }
        Write-Host "    - Total: $totalFiles files (~$([math]::Round($totalSize, 2)) MB)" -ForegroundColor Green
    }
    Write-Host "  COMPLETE: $($viz.Name)" -ForegroundColor Green
    $successCount++
    Write-Host ""
}

Write-Host "=== Update Complete ===" -ForegroundColor Cyan
Write-Host "Successfully updated: $successCount visualizers" -ForegroundColor Green
if ($skippedCount -gt 0) {
    Write-Host "Skipped (not built): $skippedCount visualizers" -ForegroundColor Yellow
}
Write-Host ""
Write-Host "All WPF visualizers have been updated in visualizers/win/WPF/" -ForegroundColor Cyan
Write-Host ""
Write-Host "Test the visualizers from: $ProjectRoot\visualizers\win\WPF\" -ForegroundColor Gray
