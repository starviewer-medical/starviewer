FORMS += \
    qexampleextensionbase.ui

HEADERS += \
    qexampleextension.h \
    exampleextensionmediator.h

SOURCES += \
    qexampleextension.cpp \
    exampleextensionmediator.cpp

RESOURCES += example.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.pri)
