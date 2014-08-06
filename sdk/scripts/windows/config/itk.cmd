REM Call gdcm configuration script to make sure GdcmDir is set
REM This script in turn calls vtk configuration script, so VtkDir will also be set
call "%ScriptsRoot%\config\gdcm.cmd"

set SourceDir=%UserProfile%\Downloads\InsightToolkit-3.20.1

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%SourceDir%-build-debug
    set InstallPrefix=%SystemDrive%/InsightToolkit/3.20.1d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%SourceDir%-build-release
    set InstallPrefix=%SystemDrive%/InsightToolkit/3.20.1
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_EXAMPLES:BOOL=FALSE ^
                 -DBUILD_TESTING:BOOL=FALSE ^
                 -DITK_USE_SYSTEM_GDCM=TRUE ^
                 -DGDCM_DIR:PATH="%GdcmDir%" ^
                 -DVTK_DIR:PATH="%VtkDir%"
