FORMS += qmprcardiac3dextensionbase.ui
HEADERS += qmprcardiac3dextension.h \
           qcardiac3dmprviewer.h \
           mprcardiac3dextensionmediator.h
SOURCES += qmprcardiac3dextension.cpp \
           qcardiac3dmprviewer.cpp \
           mprcardiac3dextensionmediator.cpp
TARGETDEPS += ../../../core/libcore.a
LIBS += ../../../core/libcore.a
INCLUDEPATH += ../../../core
MOC_DIR = ../../../../tmp/moc
UI_DIR = ../../../../tmp/ui
OBJECTS_DIR = ../../../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += warn_on \
qt \
opengl \
thread \
x11 \
staticlib \
exceptions \
stl
TEMPLATE = lib
include(../../../vtk.inc)
include(../../../itk.inc)
include(../../../dcmtk.inc)
include(../../../compilationtype.inc)
