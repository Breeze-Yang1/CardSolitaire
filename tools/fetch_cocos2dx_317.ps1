param(
    [string]$InstallDir = "$PSScriptRoot\..\third_party"
)

$ErrorActionPreference = "Stop"

$target = Join-Path $InstallDir "cocos2d-x-3.17.2"
$depsRepo = Join-Path $InstallDir "cocos2d-x-3rd-party-libs-bin"

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

if (-not (Test-Path $target)) {
    if (-not (Get-Command git.exe -ErrorAction SilentlyContinue)) {
        throw "git.exe is required. Install Git first or place Cocos2d-x at $target manually."
    }

    Write-Host "Cloning Cocos2d-x 3.17.2 from Gitee mirror..."
    git.exe clone --branch cocos2d-x-3.17.2 --depth 1 --single-branch https://gitee.com/mirrors/cocos2d-x.git $target
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to clone Cocos2d-x 3.17.2."
    }
}

$externalCmake = Join-Path $target "external\CMakeLists.txt"
if (-not (Test-Path $externalCmake)) {
    if (-not (Test-Path $depsRepo)) {
        Write-Host "Cloning Cocos2d-x 3.x third-party dependencies from Gitee mirror..."
        git.exe clone --branch v3 --depth 1 --single-branch https://gitee.com/yangyanpeng/cocos2d-x-3rd-party-libs-bin.git $depsRepo
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to clone Cocos2d-x third-party dependencies."
        }
    }

    Write-Host "Copying third-party dependencies into Cocos2d-x external directory..."
    robocopy $depsRepo (Join-Path $target "external") /E /XD .git | Out-Host
    if ($LASTEXITCODE -gt 7) {
        throw "robocopy failed with exit code $LASTEXITCODE"
    }
}

Write-Host "Cocos2d-x is ready: $target"
Write-Host "Set environment variable COCOS2DX_ROOT=$target"
