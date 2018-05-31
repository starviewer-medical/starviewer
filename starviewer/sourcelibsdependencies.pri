include(addlibrarydependency.pri)
include($$PWD/src/extensions.pri)

for(dir, PLAYGROUND_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/playground, $$OUT_PWD/../../src/extensions/playground, $$dir)
}

for(dir, CONTRIB_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/contrib, $$OUT_PWD/../../src/extensions/contrib, $$dir)
}

for(dir, MAIN_EXTENSIONS) {
    DUMMY = $$addLibraryDependency($$PWD/src/extensions/main, $$OUT_PWD/../../src/extensions/main, $$dir)
}

$$addLibraryDependency($$PWD/src/thirdparty, $$OUT_PWD/../../src/thirdparty, easylogging++)

# Dependències de llibreries core
DUMMY = $$addLibraryDependency($$PWD/src, $$OUT_PWD/../../src, interface)
linux:LIBS += -Wl,--start-group
DUMMY = $$addLibraryDependency($$PWD/src, $$OUT_PWD/../../src, inputoutput)
DUMMY = $$addLibraryDependency($$PWD/src, $$OUT_PWD/../../src, core)
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
include($$PWD/src/cuda.pri)
include($$PWD/src/compilationtype.pri)
include($$PWD/src/threadweaver.pri)
