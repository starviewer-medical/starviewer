CONFIG += debug_and_release c++11

# TODO: remove once ITK is compiled with c++11
clang:QMAKE_CXXFLAGS += -Wno-static-float-init -Wno-c++11-narrowing
linux-g++:QMAKE_CXXFLAGS += -fpermissive

# Use gold linker
linux:QMAKE_LFLAGS += -fuse-ld=gold

unix {
    QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
    QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated

    !macx:LIBS += -lGLU
}

win32 {
    # Per tenir glew integrada al codi. Només necessari a windows.
    DEFINES += GLEW_STATIC
    # Avoid definition of min and max macros by windows.h
    DEFINES += NOMINMAX
    
    # Volem veure missatges a std::cout en debug
    CONFIG(debug, debug|release) {
        CONFIG += console
    }
    else {
        CONFIG -= console
    }
}

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    LIBS += -framework Cocoa
}

# definim que per sistemes de compilació windows basats en visual studio 
# la compilació es faci en tants cores com sigui possible

win32-msvc2013:QMAKE_CXXFLAGS += /MP

# Indiquem que per compil·lacions en debug, Runtime Library sigui Multi-threaded DLL (com en release) i no Multi-threaded Debug DLL
win32-msvc2013:QMAKE_CXXFLAGS_DEBUG -= -MDd
win32-msvc2013:QMAKE_CXXFLAGS_DEBUG += -MD

win32-msvc2013:QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings

# Definim variable per construir versió lite en temps de compilació
lite_version:DEFINES += STARVIEWER_LITE

# EasyLogging++ is set to be thread safe and to not have a default log file
DEFINES += ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE
