$ErrorActionPreference = "Stop"

& "C:\Users\casta\OneDrive\Desktop\emscripten\emsdk\emsdk_env.ps1"

$RootDir = "root"
$OutDir = "out"
$ObjDir = "out/obj"

$Includes = @(
    "C:/Program Files/Java/jdk-22/include",
    "C:/Program Files/Java/jdk-22/include/win32",
    "C:/VCPKG/vcpkg",
    "C:/msys64/mingw64",
    "C:/openssl",
    "C:/openssl/include",
    "C:/Users/casta/OneDrive/Desktop/vscode/messages/main/vcpkg/installed/x64-windows/mingw64",
    "C:/Users/casta/OneDrive/Desktop/vscode/messages/main/vcpkg/installed/x64-windows/mingw64/bin",
    "C:/Users/casta/OneDrive/Desktop/vscode/messages/main/vcpkg/installed/x64-windows/include/openssl",
    "C:/Users/casta/OneDrive/Desktop/glfw-3.4",
    "C:/Users/casta/OneDrive/Desktop/glfw-3.4/include",
    "C:/Users/casta/OneDrive/Desktop/glad",
    "C:/Users/casta/OneDrive/Desktop/glad/include",
    "C:/Users/casta/OneDrive/Desktop/glad/src",
    "C:/Users/casta/OneDrive/Desktop/emscripten/emsdk",
    "C:/Users/casta/OneDrive/Desktop/glm-1.0.2",
    "C:/Users/casta/OneDrive/Desktop/glm-1.0.2/glm",
    "C:/Users/casta/OneDrive/Documentos/dependencies/lib/cmake",
    "C:/Users/casta/cmake-3.28.0-rc3",
    "C:/Users/casta/OneDrive/Documentos/stb-master"
)

$IncludeFlags = $Includes | ForEach-Object { "-I`"$_`"" }

if(!(Test-Path $ObjDir)) { New-Item -ItemType Directory -Force -Path $ObjDir }

function Build-Project {
    $changed = 0
    $objFiles = @()

    Get-ChildItem -Path $RootDir -Recurse -Include *.cpp,*.c | Where-Object { $_.FullName -notmatch "\\out\\" } | ForEach-Object {
        $src = $_
        $relativePath = $src.FullName.Replace("$PWD\$RootDir\", "").Replace("\", "_")
        $objName = $relativePath.Replace($src.Extension, ".o")
        $objPath = Join-Path $ObjDir $objName
        
        $needsCompile = $false
        if(!(Test-Path $objPath)) {
            $needsCompile = $true
            Write-Host "New: $($src.Name)" -ForegroundColor Green
        }
        elseif($src.LastWriteTime -gt (Get-Item $objPath).LastWriteTime) {
            $needsCompile = $true
            Write-Host "Changed: $($src.Name)" -ForegroundColor Yellow
        }
        
        if($needsCompile) {
            $projectIncludes = @(
                "-I`"$RootDir`"",
                "-I`"$RootDir/buffers`"",
                "-I`"$RootDir/controller`"",
                "-I`"$RootDir/preset`"",
                "-I`"$RootDir/_data`"",
                "-I`"$RootDir/_shaders`"",
                "-I`"$RootDir/_utils`""
            )
            
            $cmd = "emcc -c `"$($src.FullName)`" -o `"$objPath`" -O0 -g $($IncludeFlags -join ' ') $($projectIncludes -join ' ')"
            Write-Host "Compiling: $($src.Name)" -ForegroundColor Cyan
            Invoke-Expression $cmd
            if($LASTEXITCODE -ne 0) { 
                Write-Host "Compile failed: $($src.Name)" -ForegroundColor Red
                return $false
            }
            $changed++
        }
        
        $objFiles += $objPath
    }

    $outJs = "out/a.out.js"
    if($changed -gt 0 -or !(Test-Path $outJs)) {
        Write-Host "Linking..." -ForegroundColor Cyan
        Write-Host "Linking $($objFiles.Count) object files" -ForegroundColor Gray
        
        if($objFiles.Count -eq 0) {
            Write-Host "No object files to link!" -ForegroundColor Red
            return $false
        }
        
        & emcc @objFiles -o $outJs -O0 -g `
            -s USE_GLFW=3 `
            -s ASYNCIFY `
            -s FETCH=1 `
            "-s" "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','UTF8ToString','stringToUTF8','lengthBytesUTF8']" `
            --preload-file $RootDir/_data@/_data `
            -s STACK_SIZE=10MB `
            -s ALLOW_MEMORY_GROWTH=1
        
        if($LASTEXITCODE -ne 0) { 
            Write-Host "Linking failed" -ForegroundColor Red
            return $false
        }
        
        Write-Host "Build complete at $(Get-Date -Format 'HH:mm:ss')" -ForegroundColor Green
    }
    else {
        Write-Host "No changes detected" -ForegroundColor Gray
    }
    
    return $true
}

Write-Host "=== Initial Build ===" -ForegroundColor Magenta
Build-Project

Write-Host "" -NoNewline
Write-Host "=== Watching for changes (Press Ctrl+C to stop) ===" -ForegroundColor Magenta
Write-Host "Monitoring: *.cpp, *.c, *.h files in $RootDir" -ForegroundColor Gray
Write-Host ""

$lastBuild = Get-Date
$debounceSeconds = 1

while($true) {
    Start-Sleep -Milliseconds 500
    
    $changedFiles = Get-ChildItem -Path $RootDir -Recurse -Include *.cpp,*.c,*.h | 
        Where-Object { 
            $_.FullName -notmatch "\\out\\" -and 
            $_.LastWriteTime -gt $lastBuild 
        }
    
    if($changedFiles) {
        Start-Sleep -Seconds $debounceSeconds
        
        Write-Host ""
        Write-Host "=== Change detected at $(Get-Date -Format 'HH:mm:ss') ===" -ForegroundColor Magenta
        $changedFiles | ForEach-Object { 
            Write-Host "  Modified: $($_.Name)" -ForegroundColor Cyan
        }
        Write-Host ""
        
        $lastBuild = Get-Date
        Build-Project
        Write-Host ""
    }
}