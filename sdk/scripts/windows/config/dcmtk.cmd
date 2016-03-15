call "%ScriptsRoot%\config\zlib.cmd"

set SourceDir=%SourceDirBase%\dcmtk-3.6.1_20120515\dcmtk-3.6.1_20120515

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%SourceDir%-build-debug
    set InstallPrefix=%SystemDrive%/dcmtk/3.6.1_20120515d-%Arch%
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%SourceDir%-build-release
    set InstallPrefix=%SystemDrive%/dcmtk/3.6.1_20120515-%Arch%
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
				 -DDCMTK_WITH_PRIVATE_TAGS:BOOL="1" ^
				 -DDCMTK_WITH_ZLIB:BOOL="1" ^
				 -DWITH_ZLIBINC:PATH="%ZlibDir%" ^
				 -DDCMTK_WITH_SNDFILE:BOOL="0" 
