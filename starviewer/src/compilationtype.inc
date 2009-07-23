CONFIG += debug_and_release

unix {
	QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
	QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
}

win32 {
    # Per tenir glew integrada al codi. Només necessari a windows.
    DEFINES += GLEW_STATIC
    
    # Volem veure missatges a std::cout en debug
    debug:CONFIG += console
    release:CONFIG -= console
}
	
# definim que per sistemes de compilació windows basats en visual studio 
# la compilació es faci en tants cores com sigui possible

win32-msvc2008:QMAKE_CXXFLAGS += /MP

# Definim variable per construir versió lite en temps de compilació
lite_version:DEFINES += STARVIEWER_LITE




