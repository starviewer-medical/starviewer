include($$PWD/src/extensions.inc)

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
DUMMY = $$addLibraryDependency($$PWD/src, inputoutput)
DUMMY = $$addLibraryDependency($$PWD/src, core)

win32{
  LIBS += -ladvapi32 \
          -lRpcrt4 \
          -lwbemuuid \
          -lIphlpapi
}

include($$PWD/src/dcmtk.inc)
include($$PWD/src/vtk.inc)
include($$PWD/src/itk.inc)
include($$PWD/src/gdcm.inc)
include($$PWD/src/log4cxx.inc)
include($$PWD/src/cuda.inc)
include($$PWD/src/compilationtype.inc)
include($$PWD/src/threadweaver.inc)
