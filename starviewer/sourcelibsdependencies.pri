include($$PWD/src/extensions.pri)

# Funció per afegir una llibreria estàtica com a dependència
defineReplace(addLibraryDependency) {
    directoryName = $$1
    libraryName = $$2
    exists($$directoryName/$$libraryName) {
        unix:PRE_TARGETDEPS += $$directoryName/$$libraryName/lib$${libraryName}.a
        win32:PRE_TARGETDEPS += $$directoryName/$$libraryName/$${libraryName}.lib
        LIBS += -L$$directoryName/$$libraryName -l$${libraryName}
        INCLUDEPATH += $$directoryName/$$libraryName
        DEPENDPATH += $$directoryName/$$libraryName
    }
    # Propaguem els canvis a fora de la funció
    export(PRE_TARGETDEPS)
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    return(0)
}

for(dir, PLAYGROUND_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/playground, $$dir)
}

for(dir, CONTRIB_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/contrib, $$dir)
}

for(dir, MAIN_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/main, $$dir)
}

# Dependències de llibreries core
DUMMY = $$addLibraryDependency($$PWD/src, interface)
linux:LIBS += -Wl,--start-group
DUMMY = $$addLibraryDependency($$PWD/src, inputoutput)
DUMMY = $$addLibraryDependency($$PWD/src, core)
linux:LIBS += -Wl,--end-group

win32{
  LIBS += -ladvapi32 \
          -lRpcrt4 \
          -lwbemuuid \
          -lIphlpapi
}

include($$PWD/src/dcmtk.pri)
include($$PWD/src/vtk.pri)
include($$PWD/src/itk.pri)
include($$PWD/src/gdcm.pri)
include($$PWD/src/log4cxx.pri)
include($$PWD/src/cuda.pri)
include($$PWD/src/compilationtype.pri)
include($$PWD/src/threadweaver.pri)
