param(
    [switch]$Reconfigure,
    [switch]$Rebuild,
    [switch]$BuildOnly
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $repoRoot "build/DebugWT"
$config = "Debug"
$exePath = Join-Path $buildDir "bin/$config/$config/OcctImgui.exe"

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command
    )
    Write-Host ">> $Command"
    Invoke-Expression $Command
}

if ($Reconfigure -or -not (Test-Path (Join-Path $buildDir "CMakeCache.txt"))) {
    Invoke-Step "cmake -S `"$repoRoot`" -B `"$buildDir`" -G `"Visual Studio 17 2022`" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Debug"
}

if ($Rebuild -and (Test-Path $buildDir)) {
    Invoke-Step "cmake --build `"$buildDir`" --config $config --target clean"
}

if (-not (Test-Path $exePath) -or $Rebuild -or $Reconfigure) {
    Invoke-Step "cmake --build `"$buildDir`" --config $config --target OcctImgui"
}

$pathCandidates = @(
    (Join-Path $buildDir "vcpkg_installed/x64-windows/debug/bin"),
    (Join-Path $buildDir "vcpkg_installed/x64-windows/bin"),
    (Join-Path $buildDir "bin/$config/$config"),
    (Join-Path $repoRoot "IFR/installed/standalone")
)

$existingPaths = @()
foreach ($candidate in $pathCandidates) {
    if (Test-Path $candidate) {
        $existingPaths += $candidate
    }
}

if ($existingPaths.Count -gt 0) {
    $env:PATH = ($existingPaths -join ";") + ";" + $env:PATH
}

if ($BuildOnly) {
    Write-Host "Build completed. Exe: $exePath"
    exit 0
}

if (-not (Test-Path $exePath)) {
    throw "OcctImgui.exe not found: $exePath"
}

Push-Location (Split-Path $exePath -Parent)
try {
    Write-Host "Launching: $exePath"
    & $exePath
    exit $LASTEXITCODE
}
finally {
    Pop-Location
}
