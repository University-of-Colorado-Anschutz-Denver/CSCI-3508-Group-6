# Builds a Release QuadM and packages it, with Qt and ExifTool, into an MSI.
# Usage (PowerShell, from the code folder):  .\packaging\build-windows-msi.ps1 -Version 1.0.0
# Output: code\dist\QuadM-<version>-windows-x64.msi
# Needs: Qt 6.12 MinGW at C:\Qt (same as CMakePresets.json), the .NET SDK, and ExifTool in third_party\exiftool.
param(
    [string]$Version = "1.0.0",   # must be numeric: major.minor.build
    [string]$QtDir = "C:/Qt/6.12.0/mingw_64",
    [string]$MinGwDir = "C:/Qt/Tools/mingw1310_64",
    [string]$NinjaDir = "C:/Qt/Tools/Ninja"
)
$ErrorActionPreference = "Stop"

if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw "Version must look like 1.2.3" }

$code = Split-Path -Parent $PSScriptRoot
Set-Location $code

if (-not (Test-Path "third_party\exiftool\exiftool.exe")) {
    throw "ExifTool is missing. Follow third_party\exiftool\README.md (Windows section) first."
}

$env:PATH = "$MinGwDir/bin;$NinjaDir;$QtDir/bin;$env:PATH"

# WiX 5 is pinned on purpose: WiX 7+ needs an OSMF fee agreement for commercial use.
if (-not (Get-Command wix -ErrorAction SilentlyContinue)) {
    Write-Host "==> Installing the WiX tool"
    dotnet tool install --global wix --version 5.0.2
    $env:PATH += ";$env:USERPROFILE\.dotnet\tools"
}
# Adds the installer wizard screens (Welcome, Finish, ...)
if (-not (wix extension list --global | Select-String "WixToolset.UI.wixext")) {
    wix extension add --global WixToolset.UI.wixext/5.0.2
}

$build = "build\release-pkg"
$stage = "$build\stage"
$dist = "dist"

Write-Host "==> Building QuadM $Version (Release)"
cmake -S . -B $build -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$QtDir" "-DCMAKE_CXX_COMPILER=$MinGwDir/bin/g++.exe"
if ($LASTEXITCODE) { throw "CMake configure failed" }
cmake --build $build
if ($LASTEXITCODE) { throw "Build failed" }

Write-Host "==> Collecting the files to install"
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory $stage | Out-Null
Copy-Item "$build\QuadM.exe" $stage
& "$QtDir/bin/windeployqt.exe" --release --no-translations --no-system-d3d-compiler --no-opengl-sw "$stage\QuadM.exe"
if ($LASTEXITCODE) { throw "windeployqt failed" }

# Same layout the app expects: <install folder>\exiftool\exiftool.exe (+ exiftool_files\)
$exif = "$stage\exiftool"
New-Item -ItemType Directory $exif | Out-Null
Get-ChildItem third_party\exiftool -Force | Where-Object { $_.Name -notin "README.md", ".gitkeep" } |
    Copy-Item -Destination $exif -Recurse

Write-Host "==> Creating the MSI"
New-Item -ItemType Directory $dist -Force | Out-Null
$msi = "$dist\QuadM-$Version-windows-x64.msi"
$stageFull = (Resolve-Path $stage).Path   # WiX resolves relative harvest paths from the .wxs folder
wix build packaging\QuadM.wxs -arch x64 -ext WixToolset.UI.wixext -d "Version=$Version" -d "StageDir=$stageFull" -o $msi
if ($LASTEXITCODE) { throw "wix build failed" }

Write-Host ""
Write-Host "Done: $code\$msi"
Get-FileHash $msi -Algorithm SHA256 | Format-List Hash
