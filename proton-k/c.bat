@echo off
mingw32-make test3d.exe
if errorlevel 1 goto err
test3d.exe
:err
rem pause
