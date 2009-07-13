CONFIG += debug_and_release

unix {
	QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
	QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
}

win32 {
    # TODO: Treure al passar a versió vtk 5.0.x pura
    DEFINES += VTK_QT_5_0_SUPPORT
    
    # Per tenir glew integrada al codi. Només necessari a windows.
    DEFINES += GLEW_STATIC
    
    # Volem veure missatges a std::cout en debug
    debug {
		CONFIG += console
	}
    release {
        CONFIG -= console
    }
}

macx {
    DEFINES += VTK_QT_5_0_SUPPORT
}
	
# definim que per sistemes de compilació windows basats en visual studio 
# la compilació es faci en tants cores com sigui possible

win32-msvc2005:QMAKE_CXXFLAGS += /MP

# Definim variable per construir versió lite en temps de compilació
lite_version:DEFINES += STARVIEWER_LITE




