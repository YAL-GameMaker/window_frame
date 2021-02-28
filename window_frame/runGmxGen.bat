@echo off
where /q gmxgen
if %ERRORLEVEL% EQU 0 (
    gmxgen %1 --copy %2 %3 --copy %4 %5
) else (
	copy /Y %2 %3
    echo Post-build: Warning N/A: Could not find GmxGen - extensions will not be updated automatically. See https://github.com/YAL-GameMaker-Tools/GmxGen for setup.
)