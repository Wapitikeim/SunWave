###Functions###
function Check-Command {
    param (
        [string]$commandLine
    )
    try {
        $parts = $commandLine -split " "
        & $parts[0] $parts[1..$parts.Length] > $null 2>&1
        return $true
    } catch {
        return $false
    }
}

# Check if cmake is available
# Check if Visual Studio 16 2019 is available

cmake --version
cmake -E capabilities

# Dynamically determine the current directory
$CurrPath = Get-Location

# Use dynamic paths instead of hardcoded paths
cmake -DCMAKE_BUILD_TYPE=Debug `
      -DCMAKE_C_COMPILER=cl.exe `
      -DCMAKE_CXX_COMPILER=cl.exe `
      -DCMAKE_INSTALL_PREFIX="$CurrPath\out\install\x64-debug" `
      -S"$CurrPath" `
      -B"$CurrPath\out\build\x64-debug" `
      -G "Visual Studio 16 2019"

cmake --build "$CurrPath\out\build\x64-debug" --parallel 14 --target SunWave --
Pause
