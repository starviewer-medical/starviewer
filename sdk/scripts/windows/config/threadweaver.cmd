REM Call ecm configuration script to make sure EcmDir is set
call "%ScriptsRoot%\config\ecm.cmd"

set SourceDir=%SourceDirPrefix%\threadweaver-5.3.0
set BuildDir=%BuildDirPrefix%\threadweaver-5.3.0

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/ThreadWeaver/5.3.0d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/ThreadWeaver/5.3.0
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_TESTING:BOOL=FALSE ^
                 -DECM_DIR:PATH="%EcmDir%"
