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
