set SourceDir=%SourceDirPrefix%\extra-cmake-modules-1.3.0
set BuildDir=%BuildDirPrefix%\extra-cmake-modules-1.3.0-rel
set InstallPrefix=%SourceDir%-%Arch%

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=Release ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_TESTING:BOOL=FALSE

set EcmDir=%InstallPrefix%/share/ECM/cmake
