@echo off

if "%1"=="Release" goto release
if "%1"=="Debug" goto debug
if "%1"=="Release32" goto release32
if "%1"=="Debug32" goto debug32
if "%1"=="clean" goto clean

echo build [Release ^| Release32 ^| Debug ^| Debug32 ^| clean]

goto out

:release
devenv exlib.sln /build "Release|x64"
goto out

:debug
devenv exlib.sln /build "Debug|x64"
goto out

:release32
devenv exlib.sln /build "Release|Win32"
goto out

:debug32
devenv exlib.sln /build "Debug|Win32"
goto out

:clean
devenv exlib.sln /clean

:out
