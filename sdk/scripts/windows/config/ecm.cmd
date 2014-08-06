set SourceDir=%UserProfile%\Downloads\extra-cmake-modules-1.0.0

set BuildDir=%SourceDir%
set InstallPrefix=%SystemDrive%/extra-cmake-modules/1.0.0

REM ============== Nothing should need to be changed below this line ==============

set CMakeOptions=-DCMAKE_BUILD_TYPE:STRING=Release ^
                 -DCMAKE_INSTALL_PREFIX:PATH="%InstallPrefix%" ^
                 -DBUILD_TESTING:BOOL=FALSE

set EcmDir=%InstallPrefix%/share/ECM/cmake
