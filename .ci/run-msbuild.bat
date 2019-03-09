nuget restore build\msvs\libtcod.sln
msbuild /nologo build\msvs\libtcod.sln /maxcpucount:2 /p:Configuration^=Debug /p:Platform^=%MSBUILD%
