set SourceDir=%SystemDrive%\buildLibs-vs2013\dcmtk-3.6.1_20120515\dcmtk-3.6.1_20120515

if %BuildType% == debug (
    set CMakeBuildType=Debug
    set BuildDir=%SourceDir%-build-debug
    set InstallPrefix=%SystemDrive%/dcmtk/3.6.1_20120515d
)
if %BuildType% == release (
    set CMakeBuildType=RelWithDebInfo
    set BuildDir=%SourceDir%-build-release
    set InstallPrefix=%SystemDrive%/dcmtk/3.6.1_20120515
)

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=%CMakeBuildType% ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
				 -DDCMTK_WITH_PRIVATE_TAGS:BOOL="1" ^
				 -DDCMTK_WITH_ZLIB:BOOL="1" ^
				 -DWITH_ZLIBINC:PATH="%SourceDir%/../dcmtk-3.6.0-win32-i386-support_MD/zlib-1.2.5" ^
				 -DDCMTK_WITH_TIFF:BOOL="1" ^
				 -DWITH_LIBTIFFINC:PATH="%SourceDir%/../dcmtk-3.6.0-win32-i386-support_MD/tiff-3.9.4" ^
				 -DDCMTK_WITH_PNG:BOOL="1" ^
				 -DWITH_LIBPNGINC:PATH="%SourceDir%/../dcmtk-3.6.0-win32-i386-support_MD/libpng-1.4.3" ^
				 -DDCMTK_WITH_SNDFILE:BOOL="0" 
