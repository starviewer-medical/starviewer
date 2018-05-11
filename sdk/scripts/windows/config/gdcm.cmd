REM Call vtk configuration script to make sure VtkDir is set
call "%ScriptsRoot%\config\vtk.cmd"

set SourceDir=%SourceDirPrefix%\GDCM-2.8.6
set BuildDir=%BuildDirPrefix%\GDCM-2.8.6

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/gdcm/2.8.6d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/gdcm/2.8.6
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DGDCM_BUILD_SHARED_LIBS:BOOL=TRUE ^
                 -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=FALSE ^
                 -DGDCM_USE_VTK:BOOL=TRUE ^
                 -DVTK_DIR="%VtkDir%"

set GdcmDir=%InstallPrefix%/lib/gdcm-2.8
