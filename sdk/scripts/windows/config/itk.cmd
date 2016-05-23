REM Call gdcm configuration script to make sure GdcmDir is set
REM This script in turn calls vtk configuration script, so VtkDir will also be set
call "%ScriptsRoot%\config\gdcm.cmd"

set SourceDir=%SourceDirPrefix%\ITK-4.7.1
set BuildDir=%BuildDirPrefix%\ITK-4.7.1

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/InsightToolkit/4.7.1d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/InsightToolkit/4.7.1
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_EXAMPLES:BOOL=FALSE ^
                 -DBUILD_TESTING:BOOL=FALSE ^
                 -DITK_USE_SYSTEM_GDCM=TRUE ^
                 -DGDCM_DIR:PATH="%GdcmDir%" ^
                 -DVTK_DIR:PATH="%VtkDir%"
