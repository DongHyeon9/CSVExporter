@echo off
cd /d %~dp0

rmdir /s /q Binaries
rmdir /q /s Build
rmdir /q /s DerivedDataCache
rmdir /q /s Intermediate
rmdir /q /s .vs

FOR /D %%D IN (Plugins\*) DO (
    IF EXIST "%%D\Binaries" (rmdir /s /q "%%D\Binaries")
    IF EXIST "%%D\Intermediate" (rmdir /s /q "%%D\Intermediate")
)

del *.sln
echo Cleanup completed.

pause