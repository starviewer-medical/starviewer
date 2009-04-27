FORMS += qcardiac2dviewerextensionbase.ui
HEADERS += cardiac2dviewerextensionmediator.h \
           qcardiac2dviewerextension.h 
SOURCES += cardiac2dviewerextensionmediator.cpp \
           qcardiac2dviewerextension.cpp

RESOURCES += cardiac2dviewer.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
