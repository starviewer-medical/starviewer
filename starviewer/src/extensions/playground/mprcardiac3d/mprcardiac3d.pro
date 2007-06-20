FORMS += qmprcardiac3dextensionbase.ui
HEADERS += qmprcardiac3dextension.h \
           qcardiac3dmprviewer.h \
           mprcardiac3dextensionmediator.h \
           qslideronestep.h
SOURCES += qmprcardiac3dextension.cpp \
           qcardiac3dmprviewer.cpp \
           mprcardiac3dextensionmediator.cpp \
           qslideronestep.cpp
TARGETDEPS += ../../../core/libcore.a \
../../../inputoutput/libinputoutput.a \
../../../interface/libinterface.a
LIBS += ../../../inputoutput/libinputoutput.a \
../../../core/libcore.a \
../../../interface/libinterface.a
INCLUDEPATH += ../../../inputoutput \
../../../core \
../../../interface
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
