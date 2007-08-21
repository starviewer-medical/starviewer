# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/registrationframework

FORMS += qregistrationframeworkextensionbase.ui 
HEADERS += qregistrationframeworkextension.h \
           registrationframeworkextensionmediator.h 
SOURCES += qregistrationframeworkextension.cpp \
           registrationframeworkextensionmediator.cpp 

TARGETDEPS += ../../../core/libcore.a \
            ../../../interface/libinterface.a \  
            ../../../inputoutput/libinputoutput.a
            
LIBS += ../../../core/libcore.a \
        ../../../interface/libinterface.a \
        ../../../inputoutput/libinputoutput.a

INCLUDEPATH += ../../../core \
            ../../../interface \
            ../../../inputoutput

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
