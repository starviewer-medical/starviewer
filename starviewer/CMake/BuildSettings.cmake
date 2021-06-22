set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)               # add source and binary directories of each folder to its own include path (i.e. include headers in core from core)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)  # add source and binary directories of each target to its interface (i.e. if main links to core, core folder is added to main include path)

# TODO check if all these options are still really needed. Remove this comment when they are tested and verified. Cross-check with original qmake settings if something fails.

# EasyLogging++ is set to be thread safe, to not have a default log file, and to not handle crashes
add_compile_definitions(ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE ELPP_DISABLE_DEFAULT_CRASH_HANDLING)

if(UNIX)
    if(APPLE)
        set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)

        # Use libc++ in macOS
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)
    else()
        # Use gold linker in Linux
        add_link_options(-fuse-ld=gold)
    endif()
elseif(WIN32)
    # Static GLEW and avoid definition of min and max macros by windows.h
    add_compile_definitions(GLEW_STATIC NOMINMAX)

    if(MSVC)
        # Multi-core compilation and generate multi-threaded DLL
        add_compile_options(/MP -MD)
    endif()
endif()

# TODO make this the CMake way® (with configurable CMake variables, etc.). Draft below
# https://cmake.org/cmake/help/v3.20/command/option.html
#[[
if(LITE_VERSION)
    add_compile_definitions(STARVIEWER_LITE)
endif()

if(CE_MARKING)
    add_compile_definitions(STARVIEWER_CE)
endif()
]]
