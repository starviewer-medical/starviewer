REM Needed to use !VAR! instead of %VAR% to get the correct value of an environment variable at runtime inside of an if or for block
setlocal EnableDelayedExpansion

if [%1]==[] goto arch32
if %1 == 64 goto arch64
goto arch32

:arch32
	set Arch=32
	call "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
	goto endArch

:arch64
	set Arch=64
	call "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64

:endArch

setlocal

set ScriptsRoot=%CD%

call "%ScriptsRoot%\config\setup.cmd" %Arch%

for %%b in (%BuildTypes%) do (
    set BuildType=%%b

    for %%l in (%Libs%) do (
        set BuildLib=%%l
        call "%ScriptsRoot%\config\%%l.cmd"
        call "%ScriptsRoot%\libs\%%l.cmd"
    )
)

endlocal
endlocal
