REM List of build types to use. Possible values: debug, release.
set BuildTypes=release
REM List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
set Libs=dcmtk vtk gdcm itk ecm threadweaver

set CMakeGenerator=-G "NMake Makefiles"
set CMake="%ProgramFiles(x86)%\CMake\bin\cmake.exe" %CMakeGenerator%

set QtInstallPrefix=%SystemDrive%\QtProject\5.3\msvc2010_opengl
set CMAKE_PREFIX_PATH=%QtInstallPrefix%

set Git="%ProgramFiles(x86)%\Git\bin\git.exe"

set PatchesRoot=%ScriptsRoot%\..\..\patches
