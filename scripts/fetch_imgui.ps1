# fetch imgui into third_party/imgui
# run from repo root:   powershell -ExecutionPolicy Bypass -File scripts/fetch_imgui.ps1

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$dst  = Join-Path $root "third_party/imgui"

if (Test-Path (Join-Path $dst "imgui.cpp")) {
    Write-Host "imgui already present at $dst"
    exit 0
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "git not found on PATH"
    exit 1
}

New-Item -ItemType Directory -Force -Path (Join-Path $root "third_party") | Out-Null
Write-Host "cloning imgui v1.90.9 into $dst"
git clone --depth 1 --branch v1.90.9 https://github.com/ocornut/imgui.git "$dst"
Write-Host "done."
