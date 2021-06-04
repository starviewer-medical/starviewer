FORMS += qpdfextensionbase.ui

HEADERS += \
    qpdfextension.h \
    pdfextensionmediator.h

SOURCES += \
    qpdfextension.cpp \
    pdfextensionmediator.cpp

RESOURCES += pdf.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.pri)
