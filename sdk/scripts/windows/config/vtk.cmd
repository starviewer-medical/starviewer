set SourceDir=%SourceDirPrefix%\VTK-8.1.1
set BuildDir=%BuildDirPrefix%\VTK-8.1.1

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%BuildDir%-deb
    set InstallPrefix=%SdkInstallPrefix%/VTK/8.1.1d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%BuildDir%-rel
    set InstallPrefix=%SdkInstallPrefix%/VTK/8.1.1
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DModule_vtkGUISupportQt:BOOL=TRUE ^
                 -DModule_vtkGUISupportQtOpenGL:BOOL=TRUE ^
                 -DModule_vtkGUISupportQtSQL:BOOL=TRUE ^
                 -DModule_vtkGUISupportQtWebkit:BOOL=FALSE ^
                 -DModule_vtkRenderingQt:BOOL=TRUE ^
                 -DModule_vtkViewsQt:BOOL=TRUE ^
                 -DVTK_QT_VERSION:STRING=5

set VtkDir=%InstallPrefix%/lib/cmake/vtk-8.1
