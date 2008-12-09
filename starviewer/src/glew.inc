# Només per sistemes no windows. Temporal mentre no s'empaqueta GLEW per windows
!win32{
    include(defaultdirectories.inc)

    LIBS += -L$${GLEWLIBDIR} -lGLEW

    INCLUDEPATH += $${GLEWINCLUDEDIR}
}