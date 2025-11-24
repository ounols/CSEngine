# CSEngine Editor Build Script
# Usage: .\build.ps1 [-Configure] [-Clean] [-Config Debug|Release]

param(
    [switch]$Configure,  # Run CMake configure
    [switch]$Clean,      # Clean build directory before configure
    [string]$Config = "Debug"  # Build configuration
)

$ErrorActionPreference = "Stop"

# Project paths - find CSEngine root
$ScriptPath = $MyInvocation.MyCommand.Path
if (!$ScriptPath) { $ScriptPath = $PSCommandPath }
$ScriptDir = Split-Path -Parent $ScriptPath
$ProjectRoot = [System.IO.Path]::GetFullPath("$ScriptDir\..\..")
$EditorPlatform = "$ProjectRoot\Editor\platforms\Windows"
$BuildDir = "$EditorPlatform\build"

# Find CMake
function Find-CMake {
    # Check PATH first
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if ($cmake) {
        return $cmake.Source
    }

    # Search Visual Studio installations
    $vsLocations = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
    )

    foreach ($vs in $vsLocations) {
        $cmakePath = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        if (Test-Path $cmakePath) {
            return $cmakePath
        }
    }

    throw "CMake not found! Install Visual Studio 2022 with C++ CMake tools or add cmake to PATH."
}

# Main
Write-Host "=== CSEngine Editor Build ===" -ForegroundColor Cyan
Write-Host "Project Root: $ProjectRoot"
Write-Host "Build Config: $Config"

# Kill running CSEditor process before build
$existingProcess = Get-Process -Name "CSEditor" -ErrorAction SilentlyContinue
if ($existingProcess) {
    Write-Host "Stopping running CSEditor (PID: $($existingProcess.Id))..." -ForegroundColor Yellow
    $existingProcess | Stop-Process -Force
    Start-Sleep -Seconds 1
    Write-Host "CSEditor stopped." -ForegroundColor Green
}

# Find CMake
$cmake = Find-CMake
Write-Host "CMake found: $cmake" -ForegroundColor Green

# Clean if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir
}

# Configure if requested or if build directory doesn't exist
if ($Configure -or !(Test-Path "$BuildDir\CSEditor.sln")) {
    Write-Host "`nConfiguring CMake..." -ForegroundColor Cyan

    & $cmake -G "Visual Studio 17 2022" -A x64 `
        -S $EditorPlatform `
        -B $BuildDir

    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed!"
    }
    Write-Host "CMake configure completed." -ForegroundColor Green
}

# Build
Write-Host "`nBuilding CSEditor ($Config)..." -ForegroundColor Cyan

& $cmake --build $BuildDir --config $Config -j 10

if ($LASTEXITCODE -ne 0) {
    throw "Build failed!"
}

$exePath = "$EditorPlatform\x64\$Config\CSEditor.exe"
if (Test-Path $exePath) {
    Write-Host "`nBuild successful!" -ForegroundColor Green
    Write-Host "Executable: $exePath"
} else {
    Write-Host "`nWarning: Build completed but executable not found at expected location." -ForegroundColor Yellow
}
