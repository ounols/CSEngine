# CSEngine Editor Launch Script
# Usage: .\run-editor.ps1 [-Config Debug|Release] [-Wait] [-WaitApi]

param(
    [string]$Config = "Debug",  # Build configuration
    [switch]$Wait,              # Wait for process to exit
    [switch]$WaitApi,           # Wait until API is ready
    [int]$ApiTimeout = 30       # API ready timeout in seconds
)

$ErrorActionPreference = "Stop"

# Project paths - find CSEngine root
$ScriptPath = $MyInvocation.MyCommand.Path
if (!$ScriptPath) { $ScriptPath = $PSCommandPath }
$ScriptDir = Split-Path -Parent $ScriptPath
$ProjectRoot = [System.IO.Path]::GetFullPath("$ScriptDir\..\..")
$EditorDir = "$ProjectRoot\Editor\platforms\Windows\x64\$Config"
$ExePath = "$EditorDir\CSEditor.exe"

# Check executable exists
if (!(Test-Path $ExePath)) {
    Write-Host "Error: CSEditor.exe not found at $ExePath" -ForegroundColor Red
    Write-Host "Run build.ps1 first to build the editor." -ForegroundColor Yellow
    exit 1
}

Write-Host "=== CSEngine Editor ===" -ForegroundColor Cyan
Write-Host "Executable: $ExePath"

# Check if already running
$existing = Get-Process -Name "CSEditor" -ErrorAction SilentlyContinue
if ($existing) {
    Write-Host "Warning: CSEditor is already running (PID: $($existing.Id))" -ForegroundColor Yellow
    $response = Read-Host "Kill existing process? (y/N)"
    if ($response -eq 'y' -or $response -eq 'Y') {
        $existing | Stop-Process -Force
        Start-Sleep -Seconds 1
    } else {
        Write-Host "Exiting without launching new instance."
        exit 0
    }
}

# Launch editor
Write-Host "Launching CSEditor..." -ForegroundColor Green
$process = Start-Process -FilePath $ExePath -WorkingDirectory $EditorDir -PassThru

Write-Host "Started with PID: $($process.Id)"

# Wait for API if requested
if ($WaitApi) {
    Write-Host "Waiting for API to be ready..." -ForegroundColor Cyan
    $apiUrl = "http://localhost:8080/api/health"
    $startTime = Get-Date
    $attempt = 0

    while ($true) {
        $attempt++
        try {
            # Use -UseBasicParsing to avoid IE dependency
            $response = Invoke-WebRequest -Uri $apiUrl -UseBasicParsing -TimeoutSec 3 -ErrorAction Stop
            $jsonBody = $response.Content | ConvertFrom-Json

            if ($jsonBody.status -eq "ok") {
                Write-Host ""
                Write-Host "API is ready! (attempt $attempt)" -ForegroundColor Green
                break
            }
        } catch {
            $errorMsg = $_.Exception.Message
            # Silently continue on connection errors
        }

        $elapsed = ((Get-Date) - $startTime).TotalSeconds
        if ($elapsed -ge $ApiTimeout) {
            Write-Host ""
            Write-Host "Warning: API did not become ready within $ApiTimeout seconds (tried $attempt times)" -ForegroundColor Yellow
            Write-Host "The editor may still be initializing. You can manually check with:" -ForegroundColor Yellow
            Write-Host "  Invoke-WebRequest http://localhost:8080/api/health" -ForegroundColor Cyan
            break
        }

        Write-Host "." -NoNewline
        Start-Sleep -Milliseconds 500
    }
}

# Wait for process if requested
if ($Wait) {
    Write-Host "Waiting for editor to exit..."
    $process.WaitForExit()
    Write-Host "Editor exited with code: $($process.ExitCode)"
}

# Output useful info
Write-Host "`nAPI Endpoint: http://localhost:8080" -ForegroundColor Cyan
Write-Host "Use api.ps1 to interact with the editor."
