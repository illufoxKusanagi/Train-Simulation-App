<#
.SYNOPSIS
  Cross-platform runner for Train Simulation App (PowerShell)

.DESCRIPTION
  Supports development and production workflows on Windows using PowerShell.

#>

param(
  [string]$Mode = 'dev'
)

function Get-RepoRoot {
  $scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Definition
  return Resolve-Path (Join-Path $scriptPath '..')
}

$ROOT = (Get-RepoRoot).Path
$FRONTEND = Join-Path $ROOT 'frontend'
$BACKEND = Join-Path $ROOT 'backend'
$BUILD = Join-Path $BACKEND 'build'

function Ensure-BuildDir {
  if (-not (Test-Path $BUILD)) { New-Item -ItemType Directory -Path $BUILD | Out-Null }
}

function Start-FrontendDev {
  Write-Output '➡️  Starting Next.js frontend (dev)'
  Push-Location $FRONTEND
  npm install
  Start-Process -NoNewWindow -FilePath npm -ArgumentList 'run','dev'
  Pop-Location
}

function Start-BackendDev {
  Write-Output '➡️  Building & starting Qt backend (dev)'
  Ensure-BuildDir
  Push-Location $BUILD
  cmake ..
  cmake --build .
  $exe1 = Join-Path $BUILD 'bin\TrainSimulationApp.exe'
  $exe2 = Join-Path $BUILD 'TrainSimulationApp.exe'
  if (Test-Path $exe1) { Start-Process -FilePath $exe1 -NoNewWindow }
  elseif (Test-Path $exe2) { Start-Process -FilePath $exe2 -NoNewWindow }
  else { Write-Warning 'Could not find built executable' }
  Pop-Location
}

function Build-Frontend {
  Push-Location $FRONTEND
  npm install
  npm run build
  Pop-Location
}

function Build-Backend {
  Ensure-BuildDir
  Push-Location $BUILD
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build .
  Pop-Location
}

switch ($Mode) {
  'dev' { Start-FrontendDev; Start-BackendDev }
  'prod' { Build-Frontend; Build-Backend }
  'build-frontend' { Build-Frontend }
  'build-backend' { Build-Backend }
  default { Write-Output "Usage: ./scripts/run.ps1 -Mode dev|prod|build-frontend|build-backend" }
}
