# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/interface
# L'objectiu és una biblioteca:  

FORMS += qconfigurationscreenbase.ui \
         queryscreenbase.ui \
         qpacslistbase.ui \
         qstudytreewidgetbase.ui \
         qserieslistwidgetbase.ui \
         qcreatedicomdirbase.ui \
         qoperationstatescreenbase.ui \
         qchooseoneobjectdialogbase.ui
TRANSLATIONS += interface_ca_ES.ts \
                interface_es_ES.ts \
                interface_en_GB.ts 
HEADERS += qapplicationmainwindow.h \
           appimportfile.h \
           extensionhandler.h \
           extensionworkspace.h \
           qconfigurationscreen.h \
           qpacslist.h \
           queryscreen.h \
           qstudytreewidget.h \
           qserieslistwidget.h \
           qcreatedicomdir.h \
           qoperationstatescreen.h \
           qchooseoneobjectdialog.h 
SOURCES += qapplicationmainwindow.cpp \
           appimportfile.cpp \
           extensionhandler.cpp \
           extensionworkspace.cpp \
           qconfigurationscreen.cpp \
           qpacslist.cpp \
           queryscreen.cpp \
           qstudytreewidget.cpp \
           qserieslistwidget.cpp \
           qcreatedicomdir.cpp \
           qoperationstatescreen.cpp \
           qchooseoneobjectdialog.cpp 
TARGETDEPS += ../core/libcore.a \
../../src/inputoutput/libinputoutput.a
LIBS += ../inputoutput/libinputoutput.a \
../core/libcore.a
INCLUDEPATH += ../inputoutput \
../core
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
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
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../compilationtype.inc)
