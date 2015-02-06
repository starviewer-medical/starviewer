REM Needed to use !VAR! instead of %VAR% to get the correct value of an environment variable at runtime inside of an if or for block
setlocal EnableDelayedExpansion

call "%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

setlocal

set ScriptsRoot=%CD%

call "%ScriptsRoot%\config\setup.cmd"

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
