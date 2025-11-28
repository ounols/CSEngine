# Final comprehensive test with preview capture and timestamps

$baseUrl = "http://localhost:8080/api"

Write-Output "=== Dodge Master Final Test ==="
Write-Output "Start Time: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss.fff')"
Write-Output ""

# 1. Load scene
Write-Output "[1/5] Loading DodgeMaster scene..."
$loadBody = @{ path = "Assets/Scene/DodgeMaster.scene" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/scene/load" -Method Post -ContentType "application/json" -Body $loadBody | Out-Null
Start-Sleep -Seconds 4
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - Scene loaded"

# 2. Add RenderComponents to make objects visible
Write-Output ""
Write-Output "[2/5] Adding RenderComponents (to make objects visible)..."

# Add to Player
$renderBody = @{ object = "Player"; type = "RenderComponent" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/component/add" -Method Post -ContentType "application/json" -Body $renderBody | Out-Null
Start-Sleep -Milliseconds 500

# Add to all Obstacles
for ($i = 1; $i -le 10; $i++) {
    $renderBody = @{ object = "Obstacle$i"; type = "RenderComponent" } | ConvertTo-Json
    Invoke-RestMethod -Uri "$baseUrl/component/add" -Method Post -ContentType "application/json" -Body $renderBody | Out-Null
    Start-Sleep -Milliseconds 300
}

Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - RenderComponents added"

# Save scene with RenderComponents
Write-Output ""
Write-Output "[3/5] Saving updated scene..."
$saveBody = @{ path = "Assets/Scene/DodgeMaster.scene" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/scene/save" -Method Post -ContentType "application/json" -Body $saveBody | Out-Null
Start-Sleep -Seconds 2
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - Scene saved"

# Reload scene
$loadBody = @{ path = "Assets/Scene/DodgeMaster.scene" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/scene/load" -Method Post -ContentType "application/json" -Body $loadBody | Out-Null
Start-Sleep -Seconds 3
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - Scene reloaded"

# 3. Capture editor preview (before Play)
Write-Output ""
Write-Output "[4/5] Capturing editor preview..."
try {
    $captureResult = Invoke-RestMethod -Uri "$baseUrl/editor/capture-preview?filename=preview_before_play.png" -Method Get
    Write-Output "  ✓ Preview captured: $($captureResult.filename)"
    Write-Output "  Resolution: $($captureResult.width)x$($captureResult.height)"
    Write-Output "  Path: $($captureResult.path)"
} catch {
    Write-Output "  ✗ Preview capture failed: $($_.ErrorDetails.Message)"
}

# 4. Start Play mode
Write-Output ""
Write-Output "[5/5] Starting Play mode..."
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - Starting..."
$playBody = @{ command = "play" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/editor/command" -Method Post -ContentType "application/json" -Body $playBody | Out-Null

# Wait and capture during play
Start-Sleep -Seconds 1
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - 1s elapsed"

try {
    $captureResult = Invoke-RestMethod -Uri "$baseUrl/editor/capture-preview?filename=preview_during_play_1s.png" -Method Get
    Write-Output "  ✓ Play preview 1s captured"
} catch {
    Write-Output "  ✗ Capture failed"
}

Start-Sleep -Seconds 2
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - 3s elapsed"

try {
    $captureResult = Invoke-RestMethod -Uri "$baseUrl/editor/capture-preview?filename=preview_during_play_3s.png" -Method Get
    Write-Output "  ✓ Play preview 3s captured"
} catch {
    Write-Output "  ✗ Capture failed"
}

Start-Sleep -Seconds 2
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - 5s elapsed"

try {
    $captureResult = Invoke-RestMethod -Uri "$baseUrl/editor/capture-preview?filename=preview_during_play_5s.png" -Method Get
    Write-Output "  ✓ Play preview 5s captured"
} catch {
    Write-Output "  ✗ Capture failed"
}

# Stop Play mode
Write-Output ""
Write-Output "Stopping Play mode..."
Write-Output "  Time: $(Get-Date -Format 'HH:mm:ss.fff') - Stopping..."
$stopBody = @{ command = "stop" } | ConvertTo-Json
Invoke-RestMethod -Uri "$baseUrl/editor/command" -Method Post -ContentType "application/json" -Body $stopBody | Out-Null
Start-Sleep -Seconds 2

# 5. Analyze logs with timestamps
Write-Output ""
Write-Output "=== Game Execution Log with Timestamps ==="
$logFile = Get-ChildItem "D:\Projects\CSEngine\Editor\platforms\Windows\x64\Debug\" -Filter "editor_log_*.log" -ErrorAction SilentlyContinue | 
    Sort-Object LastWriteTime -Descending | 
    Select-Object -First 1

if ($logFile) {
    Write-Output "Log file: $($logFile.Name)"
    Write-Output ""
    
    # Get logs from Play mode
    $logs = Get-Content $logFile.FullName | Select-String -Pattern "\[.*\] \[#.*\] .*" | Select-Object -Last 50
    
    # Filter game-related logs
    $gameLogs = $logs | Select-String -Pattern "(Player|Obstacle|GameManager|Score|Game Over|initialized)" 
    
    if ($gameLogs) {
        Write-Output "Game Events:"
        $gameLogs | ForEach-Object { Write-Output $_ }
    } else {
        Write-Output "No game logs found. Showing last 20 lines:"
        $logs | Select-Object -Last 20 | ForEach-Object { Write-Output $_ }
    }
}

Write-Output ""
Write-Output "=== Screenshots Captured ==="
Get-ChildItem "D:\Projects\CSEngine\" -Filter "preview_*.png" -ErrorAction SilentlyContinue | 
    Sort-Object LastWriteTime -Descending | 
    Select-Object -First 5 | 
    ForEach-Object { 
        Write-Output "  - $($_.Name) ($([math]::Round($_.Length / 1KB, 2)) KB)" 
    }

Write-Output ""
Write-Output "End Time: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss.fff')"
Write-Output "=== Test Complete ==="

