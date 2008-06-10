CONFIG += debug_and_release

unix {
	QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
	QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
}

win32 {
    # TODO: Treure al passar a versió vtk 5.0.x pura
    DEFINES += VTK_QT_5_0_SUPPORT
}
