
TARGET = autotests
DESTDIR = ./
TEMPLATE = app

CONFIG -= app_bundle

INCLUDEPATH += shared
DEPENDPATH += shared

SOURCES += autotests.cpp

QT += testlib

QT += xml opengl network xmlpatterns gui concurrent qml quick quickwidgets sql webenginewidgets

OBJECTS_DIR = $$OUT_PWD/../../tmp/obj
UI_DIR = $$OUT_PWD/../../tmp/ui
MOC_DIR = $$OUT_PWD/../../tmp/moc
RCC_DIR = $$OUT_PWD/../../tmp/rcc

include(shared/shared.pri)
include(unittests/unittests.pri)
include(../../sourcelibsdependencies.pri)
include(../../src/makefixdebug.pri)

#TODO: No afegeix la informacio de UI_DIR com a include a l'hora de compilar el testos
INCLUDEPATH += $$OUT_PWD/../../tmp/ui

RESOURCES = $$OUT_PWD/../../src/main/main.qrc
