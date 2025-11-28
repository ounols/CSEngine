# CSEngine Editor REST API Helper
# Usage: .\api.ps1 <command> [arguments]
#
# Commands:
#   health                    - Check API status
#   scene load <path>         - Load scene (use "new" for new scene)
#   scene save [name]         - Save current scene (optionally with custom name)
#   scene info                - Get current scene info
#   object list               - List all objects
#   object info <name>        - Get object details
#   object create <name>      - Create empty object
#   object transform <name> <x> <y> <z>  - Set position
#   component add <object> <type> [script]  - Add component
#   play                      - Start play mode
#   stop                      - Stop play mode
#   log [count]               - Get recent logs (default: 30)

param(
    [Parameter(Position=0)]
    [string]$Command,

    [Parameter(Position=1, ValueFromRemainingArguments=$true)]
    [string[]]$Args
)

$ErrorActionPreference = "Stop"
$BaseUrl = "http://localhost:8080/api"

function Invoke-Api {
    param(
        [string]$Endpoint,
        [string]$Method = "GET",
        [hashtable]$Body = $null
    )

    $url = "$BaseUrl/$Endpoint"
    $params = @{
        Uri = $url
        Method = $Method
        ContentType = "application/json"
    }

    if ($Body) {
        $params.Body = ($Body | ConvertTo-Json -Compress)
    }

    try {
        $response = Invoke-RestMethod @params
        return $response
    } catch {
        if ($_.Exception.Response) {
            Write-Host "API Error: $($_.Exception.Message)" -ForegroundColor Red
        } else {
            Write-Host "Connection Error: Is CSEditor running?" -ForegroundColor Red
        }
        exit 1
    }
}

function Show-Help {
    Write-Host @"
CSEngine Editor API Helper

Usage: .\api.ps1 <command> [arguments]

Commands:
  health                              Check API status
  scene load <path>                   Load scene (use "new" for new scene)
  scene save [name]                   Save current scene (optionally with custom name)
  scene info                          Get current scene info
  object list                         List all objects
  object info <name>                  Get object details
  object create <name>                Create empty object
  object transform <name> <x> <y> <z> Set object position
  component add <obj> <type> [script] Add component (types: CustomComponent, CameraComponent, LightComponent)
  play                                Start play mode
  stop                                Stop play mode
  log [count]                         Get recent logs (default: 30)

Examples:
  .\api.ps1 health
  .\api.ps1 scene load Assets/Scene/DodgeMaster.scene
  .\api.ps1 scene load new
  .\api.ps1 scene save NewDodgeMaster
  .\api.ps1 object create Player
  .\api.ps1 object transform Player 0 -2 0
  .\api.ps1 component add Player CustomComponent PlayerController
  .\api.ps1 play
  .\api.ps1 log 50
"@
}

# Main command handling
switch ($Command) {
    "health" {
        $result = Invoke-Api "health"
        Write-Host "Status: $($result.status)" -ForegroundColor Green
        Write-Host "Server: $($result.server)"
    }

    "scene" {
        switch ($Args[0]) {
            "load" {
                if (!$Args[1]) {
                    Write-Host "Usage: api.ps1 scene load <path|new>" -ForegroundColor Yellow
                    exit 1
                }
                $result = Invoke-Api "scene/load" "POST" @{ path = $Args[1] }
                Write-Host "Status: $($result.status)" -ForegroundColor Green
                Write-Host "Message: $($result.message)"
            }
            "save" {
                if ($Args[1]) {
                    $body = @{ name = $Args[1] }
                    $result = Invoke-Api "scene/save" "POST" $body
                    Write-Host "Scene saved as: $($Args[1])" -ForegroundColor Green
                } else {
                    $result = Invoke-Api "scene/save" "POST"
                    Write-Host "Scene saved!" -ForegroundColor Green
                }
            }
            "info" {
                $result = Invoke-Api "scene/info"
                Write-Host "Scene: $($result.name)"
            }
            default {
                Write-Host "Usage: api.ps1 scene <load|save|info>" -ForegroundColor Yellow
            }
        }
    }

    "object" {
        switch ($Args[0]) {
            "list" {
                $result = Invoke-Api "object/list"
                Write-Host "Objects ($($result.objects.Count)):" -ForegroundColor Cyan
                foreach ($obj in $result.objects) {
                    $status = if ($obj.enabled) { "[+]" } else { "[-]" }
                    Write-Host "  $status $($obj.name)"
                }
            }
            "info" {
                if (!$Args[1]) {
                    Write-Host "Usage: api.ps1 object info <name>" -ForegroundColor Yellow
                    exit 1
                }
                $result = Invoke-Api "object/info?name=$($Args[1])"
                $result | ConvertTo-Json -Depth 5
            }
            "create" {
                if (!$Args[1]) {
                    Write-Host "Usage: api.ps1 object create <name>" -ForegroundColor Yellow
                    exit 1
                }
                $result = Invoke-Api "object/create" "POST" @{ type = "empty"; name = $Args[1] }
                Write-Host "Created: $($Args[1])" -ForegroundColor Green
            }
            "transform" {
                if ($Args.Count -lt 4) {
                    Write-Host "Usage: api.ps1 object transform <name> <x> <y> <z>" -ForegroundColor Yellow
                    exit 1
                }
                $body = @{
                    object = $Args[1]
                    posX = [float]$Args[2]
                    posY = [float]$Args[3]
                    posZ = [float]$Args[4]
                }
                $result = Invoke-Api "object/set-transform" "POST" $body
                Write-Host "Transform set for $($Args[1])" -ForegroundColor Green
            }
            default {
                Write-Host "Usage: api.ps1 object <list|info|create|transform>" -ForegroundColor Yellow
            }
        }
    }

    "component" {
        switch ($Args[0]) {
            "add" {
                if ($Args.Count -lt 2) {
                    Write-Host "Usage: api.ps1 component add <object> <type> [scriptPath]" -ForegroundColor Yellow
                    exit 1
                }
                $body = @{
                    object = $Args[1]
                    type = $Args[2]
                }
                if ($Args[3]) {
                    $body.scriptPath = $Args[3]
                }
                $result = Invoke-Api "component/add" "POST" $body
                Write-Host "Component added to $($Args[1])" -ForegroundColor Green
            }
            default {
                Write-Host "Usage: api.ps1 component add <object> <type> [scriptPath]" -ForegroundColor Yellow
            }
        }
    }

    "play" {
        $result = Invoke-Api "editor/command" "POST" @{ command = "play" }
        Write-Host "Play mode started!" -ForegroundColor Green
        Write-Host "Resolution: $($result.resolution)"
    }

    "stop" {
        $result = Invoke-Api "editor/command" "POST" @{ command = "stop" }
        Write-Host "Play mode stopped." -ForegroundColor Green
    }

    "log" {
        $count = if ($Args[0]) { $Args[0] } else { 30 }
        $result = Invoke-Api "log/recent?count=$count"

        if ($result -is [array]) {
            foreach ($entry in $result) {
                $color = switch ($entry.severity) {
                    2 { "Yellow" }  # Warning
                    3 { "Red" }     # Error
                    default { "White" }
                }
                Write-Host "[$($entry.timestamp)] $($entry.details)" -ForegroundColor $color
            }
        } else {
            Write-Host "[$($result.timestamp)] $($result.details)"
        }
    }

    default {
        Show-Help
    }
}
