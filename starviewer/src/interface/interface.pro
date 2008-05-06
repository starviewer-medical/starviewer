# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/interface
# L'objectiu és una biblioteca:  

TRANSLATIONS += interface_ca_ES.ts \
                interface_es_ES.ts \
                interface_en_GB.ts 
FORMS += qconfigurationdialogbase.ui
HEADERS += qapplicationmainwindow.h \
           appimportfile.h \
           extensionhandler.h \
           extensionworkspace.h \
           qconfigurationdialog.h
SOURCES += qapplicationmainwindow.cpp \
           appimportfile.cpp \
           extensionhandler.cpp \
           extensionworkspace.cpp \
           qconfigurationdialog.cpp

INCLUDEPATH += ../inputoutput \
               ../core
DEPENDPATH += ../inputoutput \
              ../core
TEMPLATE = lib

DESTDIR = ./

include(../corelibsconfiguration.inc)
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)
