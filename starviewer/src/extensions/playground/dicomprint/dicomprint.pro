# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/dicomprint
# L'objectiu � una biblioteca:

FORMS += qdicomprintextensionbase.ui \
         qdicomjobcreatorwidgetbase.ui \
         qdicomprinterconfigurationwidgetbase.ui \
         qdicomaddprinterwidgetbase.ui \
         qdicomprinterbasicsettingswidgetbase.ui
         
HEADERS +=  qdicomprintextension.h \
            dicomprintextensionmediator.h \
            imageprintfactory.h \
            dicomprintfactory.h \
            qprinterconfigurationwidget.h \
            qdicomprinterconfigurationwidget.h \
            printermanager.h \
            dicomprintermanager.h \
            printer.h \
            dicomprinter.h \
            qprintingconfigurationwidget.h \
            qprintjobcreatorwidget.h \
            qdicomjobcreatorwidget.h \
            printjob.h \
            dicomprintjob.h \
            printmethod.h \
            dicomprintmethod.h \
            printpage.h \
            dicomprintpage.h \
            fakeobjects.h \
            dicomprint.h \
            createdicomprintspool.h \
            printdicomspool.h \
            qdicomaddprinterwidget.h \
            qdicomprinterbasicsettingswidget.h
            
SOURCES +=  qdicomprintextension.cpp \
            dicomprintextensionmediator.cpp \
            dicomprintfactory.cpp \
            qdicomprinterconfigurationwidget.cpp \
            dicomprinter.cpp \
            dicomprintermanager.cpp \
            qdicomjobcreatorwidget.cpp \
            printjob.cpp \
            dicomprintjob.cpp \
            dicomprintmethod.cpp \
            printpage.cpp \
            dicomprintpage.cpp \
            fakeobjects.cpp \
            dicomprint.cpp \
            createdicomprintspool.cpp \
            printdicomspool.cpp \
            qdicomaddprinterwidget.cpp \
            qdicomprinterbasicsettingswidget.cpp

RESOURCES += dicomprint.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
