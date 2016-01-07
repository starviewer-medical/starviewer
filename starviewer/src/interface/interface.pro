# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/interface
# L'objectiu és una biblioteca:  

TRANSLATIONS += interface_ca_ES.ts \
                interface_es_ES.ts \
                interface_en_GB.ts 
FORMS += qconfigurationdialogbase.ui \
        qaboutdialogbase.ui \
        qlicensedialogbase.ui
HEADERS += qapplicationmainwindow.h \
           interfacesettings.h \
           appimportfile.h \
           extensionhandler.h \
           extensionworkspace.h \
           qconfigurationdialog.h \
           starviewerapplicationcommandline.h \
           applicationcommandlineoptions.h \
           qaboutdialog.h \
           qlicensedialog.h
SOURCES += qapplicationmainwindow.cpp \
           interfacesettings.cpp \
           appimportfile.cpp \
           extensionhandler.cpp \
           extensionworkspace.cpp \
           qconfigurationdialog.cpp \
           starviewerapplicationcommandline.cpp \
           applicationcommandlineoptions.cpp \
           qaboutdialog.cpp \
           qlicensedialog.cpp
           

INCLUDEPATH += ../inputoutput \
               ../core
DEPENDPATH += ../inputoutput \
              ../core
TEMPLATE = lib

DESTDIR = ./

CONFIG += staticlib

include(../corelibsconfiguration.pri)
include(../vtk.pri)
include(../itk.pri)
include(../dcmtk.pri)
include(../log4cxx.pri)
include(../threadweaver.pri)
include(../compilationtype.pri)

QT += widgets
