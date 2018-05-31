# Function to add an internal static library as a dependency
# Usage: $$addLibraryDependency(<librarySourceDir>, <libraryOutputDir>, <libraryName>)
defineReplace(addLibraryDependency) {
    directoryName = $$1
    outputDirectoryName = $$2
    libraryName = $$3

    exists($$directoryName/$$libraryName) {
        unix:PRE_TARGETDEPS += $$outputDirectoryName/$$libraryName/lib$${libraryName}.a
        win32:PRE_TARGETDEPS += $$outputDirectoryName/$$libraryName/$${libraryName}.lib
        LIBS += -L$$outputDirectoryName/$$libraryName -l$${libraryName}
        INCLUDEPATH += $$directoryName/$$libraryName
        DEPENDPATH += $$outputDirectoryName/$$libraryName
    }

    # Propagate changes to the outside
    export(PRE_TARGETDEPS)
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    return(0)
}
