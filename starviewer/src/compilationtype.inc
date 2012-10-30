CONFIG += debug_and_release

unix {
	QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
	QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
}

win32 {
    # Per tenir glew integrada al codi. Només necessari a windows.
    DEFINES += GLEW_STATIC
    
    # Volem veure missatges a std::cout en debug
	CONFIG(debug, debug|release) {
		CONFIG += console
	}
	else {
		CONFIG -= console
	}
}

macx {
    LIBS += -framework Cocoa
}

# definim que per sistemes de compilació windows basats en visual studio 
# la compilació es faci en tants cores com sigui possible

win32-msvc2010:QMAKE_CXXFLAGS += /MP

# Indiquem que per compil·lacions en debug, Runtime Library sigui Multi-threaded DLL (com en release) i no Multi-threaded Debug DLL
win32-msvc2010:QMAKE_CXXFLAGS_DEBUG -= -MDd
win32-msvc2010:QMAKE_CXXFLAGS_DEBUG += -MD

# Definim variable per construir versió lite en temps de compilació
lite_version:DEFINES += STARVIEWER_LITE




