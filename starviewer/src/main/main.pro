# Afegim dependències de les extensions

include(../applicationstargetnames.pri)

TARGET = $${TARGET_STARVIEWER}
DESTDIR = ../../bin
TEMPLATE = app

# CrashHandler
SOURCES += crashhandler.cpp
HEADERS += crashhandler.h

# End CrashHandler

SOURCES += main.cpp \
           syncactionsregister.cpp \
           applicationtranslationsloader.cpp
HEADERS += applicationtranslationsloader.h \
           syncactionsregister.h \
           diagnosistests.h \
           vtkinit.h
RESOURCES = main.qrc ../qml/qml.qrc

win32{
RC_FILE = starviewer.rc
}
macx {
    ICON = images/starviewer.icns
}

# Definim que per sistemes de compilació windows basats en visual studio 
# s'activi el flag /LARGEADDRESSAWARE, que permet que es puguin fer servir
# més de 2Gb de memòria per procés. Això serà efectiu en sistemes de 64 bits
win32-msvc2013:QMAKE_LFLAGS += /LARGEADDRESSAWARE

include(../../sourcelibsdependencies.pri)

# Thirdparty libraries
DUMMY = $$addLibraryDependency(../thirdparty, breakpad)

include(../corelibsconfiguration.pri)
include(../thirdparty/qtsingleapplication/src/qtsingleapplication.pri)
include(../breakpad.pri)

include(installextensions.pri)

QT += xml opengl network webkit xmlpatterns qml declarative concurrent webkitwidgets

#TODO: Qt 4.5.3 no afegeix la informacio de UI_DIR com a include a l'hora de compilar el main.cpp
INCLUDEPATH += ../../tmp/ui
