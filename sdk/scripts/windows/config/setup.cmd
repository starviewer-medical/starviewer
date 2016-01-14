REM List of build types to use. Possible values: debug, release.
set BuildTypes=release

REM List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
set Libs=zlib dcmtk vtk gdcm itk ecm threadweaver

set SourceDirBase=D:\buildLibs-vs2013-%Arch%

set CMakeGenerator=-G "NMake Makefiles"
set CMake="%ProgramFiles(x86)%\CMake\bin\cmake.exe" %CMakeGenerator%

if %Arch% == 64 (
	set QtInstallPrefix=%SystemDrive%\Qt\Qt5.4.1\5.4\msvc2013_64_opengl
	echo 64 bit
) else (
	set QtInstallPrefix=%SystemDrive%\Qt\Qt5.4.1\5.4\msvc2013_opengl
	echo 32 bit
)

set CMAKE_PREFIX_PATH=%QtInstallPrefix%

set Git="%ProgramFiles(x86)%\Git\bin\git.exe"

set PatchesRoot=%ScriptsRoot%\..\..\patches
