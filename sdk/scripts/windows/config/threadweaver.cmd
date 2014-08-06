REM Call ecm configuration script to make sure EcmDir is set
call "%ScriptsRoot%\config\ecm.cmd"

set SourceDir=%UserProfile%\Downloads\threadweaver-5.0.0

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%SourceDir%-build-debug
    set InstallPrefix=%SystemDrive%/ThreadWeaver/5.0.0d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%SourceDir%-build-release
    set InstallPrefix=%SystemDrive%/ThreadWeaver/5.0.0
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_TESTING:BOOL=FALSE ^
                 -DECM_DIR:PATH="%EcmDir%"
