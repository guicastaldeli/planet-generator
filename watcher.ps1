Write-Host "Watching for changes..."
$lastState = @{}

while ($true) {
    $changed = $false
    $files = Get-ChildItem -Recurse -Include *.c, *.cpp

    foreach ($f in $files) {
        $time = $f.LastWriteTimeUtc.Ticks

        if (!$lastState.ContainsKey($f.FullName) -or $lastState[$f.FullName] -ne $time) {
            $lastState[$f.FullName] = $time
            $changed = $true
        }
    }

    if ($changed) {
        Write-Host "Change detected! Recompiling..."
        $sources = ($files | ForEach-Object { $_.FullName }) -join " "

        Write-Host "Sources: $sources"
        $cmd = "emcc $sources -s NO_EXIT_RUNTIME=0"

        Write-Host $cmd
        iex $cmd
        Write-Host "Recompiled!" -ForegroundColor Green
    }

    Start-Sleep -Milliseconds 300
}
