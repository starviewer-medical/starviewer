REM Call zlib configuration script to make sure ZlibDir is set
call "%ScriptsRoot%\config\zlib.cmd"

set SourceDir=%SourceDirPrefix%\dcmtk-3.6.1_20120515
set BuildDir=%BuildDirPrefix%\dcmtk-3.6.1_20120515

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/dcmtk/3.6.1_20120515d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/dcmtk/3.6.1_20120515
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DDCMTK_WITH_PRIVATE_TAGS:BOOL="1" ^
                 -DDCMTK_WITH_ZLIB:BOOL="1" ^
                 -DWITH_ZLIBINC:PATH="%ZlibDir%" ^
                 -DDCMTK_WITH_SNDFILE:BOOL="0" 
