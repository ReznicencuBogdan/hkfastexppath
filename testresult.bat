@ECHO OFF
setlocal enabledelayedexpansion
set content=
for /f "delims=" %%x in ('call "D:\Local Projects\ConEmuCDScript\build\Debug\ConEmuCD.exe"') do (set content=!content! %%x)
echo %content%

pushd %content%
chdir /d %content%


cmd.exe /k "@echo off & setlocal enabledelayedexpansion & set content="" & for /f "delims=" %%x in ('call "D:\Local Projects\ConEmuCDScript\build\Debug\ConEmuCD.exe"') do (set content=!content! %%x) & echo %content%  "