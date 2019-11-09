nuget restore buildsys\msvs\libtcod.sln
msbuild /nologo buildsys\msvs\libtcod.sln /maxcpucount:2 /p:Configuration^=Debug /p:Platform^=%MSBUILD%
