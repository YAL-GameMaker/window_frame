@echo off
set dllPath=%~1
set solutionDir=%~2
set projectDir=%~3
set arch=%~4
set cppPath=%projectDir%window_frame.cpp
set ext14=%solutionDir%window_frame.gmx\extensions\window_frame
set ext22=%solutionDir%window_frame_22\extensions\window_frame
set ext23=%solutionDir%window_frame_23\extensions\window_frame
set gmlPath=%ext23%\window_frame.gml

where /q gmxgen
if %ERRORLEVEL% EQU 0 (
::
gmxgen "%ext23%\window_frame.yy" ^
--copy "%dllPath%" "window_frame.dll:%arch%" ^
--copy "%cppPath%" "window_frame.cpp"

gmxgen "%ext22%\window_frame.yy" ^
--copy "%dllPath%" "window_frame.dll:%arch%" ^
--copy "%cppPath%" "window_frame.cpp" ^
--copy "%gmlPath%" "window_frame.gml"

gmxgen "%ext14%.extension.gmx" ^
--copy "%dllPath%" "window_frame.dll:%arch%" ^
--copy "%cppPath%" "window_frame.cpp" ^
--copy "%gmlPath%" "window_frame.gml"

) else (

:: copy DLLs:
if "%arch%" EQU "x64" (
copy /Y "%dllPath%" "%ext23%\window_frame_x64.dll"
) else (
copy /Y "%dllPath%" "%ext14%\window_frame.dll"
copy /Y "%dllPath%" "%ext22%\window_frame.dll"
copy /Y "%dllPath%" "%ext23%\window_frame.dll"
)

:: copy GML files from 2.3 project to others:
copy /Y "%gmlPath%" "%ext14%\window_frame.gml"
copy /Y "%gmlPath%" "%ext22%\window_frame.gml"

echo Post-build: Warning N/A: Could not find GmxGen - extensions will not be updated automatically. See https://github.com/YAL-GameMaker-Tools/GmxGen for setup.
)