# Update Qt Visualizers for Windows
# This script copies Qt visualizers to visualizers/win/Qt/ if they are newer
# Similar to update_apps.sh for Linux

param(
    [switch]$Force,
    [switch]$Help
)

if ($Help) {
    Write-Host @"
Update Qt Visualizers for Windows

Usage: .\update_qt_visualizers.ps1 [-Force] [-Help]

Options:
  -Force        Force copy even if destination is newer
  -Help         Show this help message

This script:
1. Checks if source binaries are newer than deployed versions
2. Copies updated files to visualizers/win/Qt/ (only if newer)
3. Skips files that haven't changed

Qt visualizers updated:
- MML_ParametricCurve3D_Visualizer
- MML_RealFunctionVisualizer

Note: Run windeployqt manually after building if DLLs need updating
"@
    exit 0
}

$ErrorActionPreference = "Stop"
$QtPath = "C:\Dev\Qt\6.10.0\msvc2022_64"
$ProjectRoot = $PSScriptRoot

# Define visualizers to update
$Visualizers = @(
    @{
        Name = "MML_ParametricCurve3D_Visualizer"
        SourceDir = "Qt\MML_ParametricCurve3D_Visualizer"
        DestDir = "visualizers\win\Qt\MML_ParametricCurve3D_Visualizer"
    },
    @{
        Name = "MML_RealFunctionVisualizer"
        SourceDir = "Qt\MML_RealFunctionVisualizer"
        DestDir = "visualizers\win\Qt\MML_RealFunctionVisualizer"
    }
)

Write-Host "=== Updating Qt Visualizers for Windows ===" -ForegroundColor Cyan
Write-Host "Project Root: $ProjectRoot"
Write-Host "Mode: $(if ($Force) { 'Force copy' } else { 'Copy if newer' })"
Write-Host ""

foreach ($viz in $Visualizers) {
    Write-Host "Processing: $($viz.Name)" -ForegroundColor Yellow
    
    $sourceDir = Join-Path $ProjectRoot $viz.SourceDir
    $releaseDir = Join-Path $sourceDir "build\bin\Release"
    $destDir = Join-Path $ProjectRoot $viz.DestDir
    
    # Check if source directory exists
    if (-not (Test-Path $releaseDir)) {
        Write-Host "  ERROR: Source directory not found: $releaseDir" -ForegroundColor Red
        Write-Host "  Build the visualizer first!" -ForegroundColor Red
        continue
    }
    
    # Create destination directory if it doesn't exist
    if (-not (Test-Path $destDir)) {
        Write-Host "  Creating destination directory..." -ForegroundColor Gray
        New-Item -ItemType Directory -Path $destDir | Out-Null
    }
    
    # Get all files from source
    $sourceFiles = Get-ChildItem -Recurse -File $releaseDir
    $copiedCount = 0
    $skippedCount = 0
    $updatedCount = 0
    
    Write-Host "  Checking files..." -ForegroundColor Gray
    
    foreach ($sourceFile in $sourceFiles) {
        $relativePath = $sourceFile.FullName.Substring($releaseDir.Length + 1)
        $destFile = Join-Path $destDir $relativePath
        $destFileDir = Split-Path $destFile -Parent
        
        # Create subdirectory if needed
        if (-not (Test-Path $destFileDir)) {
            New-Item -ItemType Directory -Path $destFileDir -Force | Out-Null
        }
        
        $shouldCopy = $Force
        
        if (-not $Force) {
            if (-not (Test-Path $destFile)) {
                $shouldCopy = $true
                $copiedCount++
            } elseif ($sourceFile.LastWriteTime -gt (Get-Item $destFile).LastWriteTime) {
                $shouldCopy = $true
                $updatedCount++
            } else {
                $skippedCount++
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
        Write-Host "    - Copied: $copiedCount files (~$([math]::Round($totalSize, 1)) MB)" -ForegroundColor Green
    } else {
        if ($copiedCount -gt 0) {
            Write-Host "    - New files: $copiedCount" -ForegroundColor Green
        }
        if ($updatedCount -gt 0) {
            Write-Host "    - Updated: $updatedCount" -ForegroundColor Green
        }
        if ($skippedCount -gt 0) {
            Write-Host "    - Unchanged: $skippedCount" -ForegroundColor Gray
        }
        Write-Host "    - Total: $totalFiles files (~$([math]::Round($totalSize, 1)) MB)" -ForegroundColor Green
    }
    Write-Host "  COMPLETE: $($viz.Name)" -ForegroundColor Green
    Write-Host ""
}

Write-Host "=== Update Complete ===" -ForegroundColor Cyan
Write-Host "All Qt visualizers have been updated in visualizers/win/Qt/" -ForegroundColor Green
Write-Host ""
Write-Host "Test the visualizers:"
foreach ($viz in $Visualizers) {
    $exePath = Join-Path $ProjectRoot "$($viz.DestDir)\$($viz.Name).exe"
    Write-Host "  $exePath"
}
