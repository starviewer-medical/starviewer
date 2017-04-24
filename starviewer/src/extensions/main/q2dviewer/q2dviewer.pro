# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/q2dviewer
# L'objectiu �s una biblioteca:

FORMS += q2dviewerextensionbase.ui \
         qhangingprotocolsgroupwidgetbase.ui \
         qhangingprotocolswidgetbase.ui

HEADERS += q2dviewerextensionmediator.h \
        q2dviewerextension.h \
        q2dviewersettings.h \
        layoutmanager.h \
        tablemenu.h \
        qhangingprotocolswidget.h \
        qhangingprotocolsgroupwidget.h \
        resetviewtoanatomicalplaneqviewercommand.h \
        qmrusortedtoolbuttonwithmenu.h

SOURCES += q2dviewerextensionmediator.cpp \
        q2dviewerextension.cpp \
        q2dviewersettings.cpp \
        layoutmanager.cpp \
        tablemenu.cpp \
        qhangingprotocolswidget.cpp \
        qhangingprotocolsgroupwidget.cpp \
        resetviewtoanatomicalplaneqviewercommand.cpp \
        qmrusortedtoolbuttonwithmenu.cpp

INCLUDEPATH += ../../../inputoutput
DEPENDPATH += ../../../inputoutput

RESOURCES += q2dviewer.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.pri)
