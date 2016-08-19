REM List of build types to use. Possible values: debug, release.
set BuildTypes=release

REM List of libs to build. Possible values: zlib, dcmtk, vtk, gdcm, itk, ecm, threadweaver.
set Libs=zlib dcmtk vtk gdcm itk ecm threadweaver

REM Global prefix of the Starviewer SDK files.
set Prefix=D:\starviewer-sdk-0.14

REM Where the libraries are downloaded.
set DownloadPrefix=%Prefix%\downloads

REM Location of SDK sources to build.
set SourceDirPrefix=%Prefix%\src

REM Location of the pathes to apply on SDK libraries.
set PatchesRoot=%ScriptsRoot%\..\..\patches

REM Location where the builds will be done.
set BuildDirPrefix=%Prefix%\build%Arch%

REM Where to install the SDK libraries once compiled.
set SdkInstallPrefix=%Prefix%\%Arch%

REM The path where Qt is installed.
if %Arch% == 32 (
    set QtInstallPrefix=%SystemDrive%\Qt\5.6\msvc2015
) else (
    set QtInstallPrefix=%SystemDrive%\Qt\5.6\msvc2015_64
)

REM How to call CMake.
set CMakeGenerator=-G "NMake Makefiles"
set CMake="%ProgramFiles(x86)%\CMake\bin\cmake.exe" %CMakeGenerator%

REM How to call git.
set Git="%ProgramFiles%\Git\bin\git.exe"

REM Needed by CMake to find Qt.
set CMAKE_PREFIX_PATH=%QtInstallPrefix%
