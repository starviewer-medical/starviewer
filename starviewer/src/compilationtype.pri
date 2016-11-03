CONFIG += debug_and_release c++11

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

    # Required since Qt 5.6 to use OpenGl directly
    LIBS += opengl32.lib glu32.lib
    # Required since Qt 5.6 by ITKCommon
    LIBS += gdi32.lib

    # definim que per sistemes de compilació windows basats en visual studio
    # la compilació es faci en tants cores com sigui possible
    QMAKE_CXXFLAGS += /MP

    # Indiquem que per compil·lacions en debug, Runtime Library sigui Multi-threaded DLL (com en release) i no Multi-threaded Debug DLL
    QMAKE_CXXFLAGS_DEBUG -= -MDd
    QMAKE_CXXFLAGS_DEBUG += -MD

    QMAKE_CXXFLAGS -= -Zc:strictStrings
}

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
    LIBS += -framework Cocoa
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
}

# Definim variable per construir versió lite en temps de compilació
lite_version:DEFINES += STARVIEWER_LITE

# EasyLogging++ is set to be thread safe, to not have a default log file, and to not handle crashes
DEFINES += ELPP_THREAD_SAFE ELPP_NO_DEFAULT_LOG_FILE ELPP_DISABLE_DEFAULT_CRASH_HANDLING
