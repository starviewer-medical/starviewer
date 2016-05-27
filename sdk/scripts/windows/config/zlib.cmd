set SourceDir=%SourceDirPrefix%\zlib-1.2.5
set BuildDir=%BuildDirPrefix%\zlib-1.2.5

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SourceDir%d-%Arch%
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SourceDir%-%Arch%
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%"

set ZlibDir=%InstallPrefix%
