# one-shot build for madcheats.cc external
# powershell -ExecutionPolicy Bypass -File scripts/build.ps1

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path "third_party/imgui/imgui.cpp")) {
    & "$PSScriptRoot/fetch_imgui.ps1"
}

cmake -S . -B build -A x64
cmake --build build --config Release --parallel
Write-Host ""
Write-Host "built -> build/bin/madcheats.cc.exe"
