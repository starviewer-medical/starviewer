REM Call vtk configuration script to make sure VtkDir is set
call "%ScriptsRoot%\config\vtk.cmd"

set SourceDir=%SourceDirPrefix%\gdcm-2.6.4
set BuildDir=%BuildDirPrefix%\gdcm-2.6.4

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/gdcm/2.6.4d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/gdcm/2.6.4
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE ^
                 -DGDCM_USE_VTK:BOOL=TRUE ^
                 -DVTK_DIR="%VtkDir%"

set GdcmDir=%InstallPrefix%/lib/gdcm-2.6
