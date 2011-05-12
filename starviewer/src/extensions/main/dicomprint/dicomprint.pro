# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/dicomprint
# L'objectiu � una biblioteca:

FORMS += qdicomprintextensionbase.ui \
         qdicomprinterconfigurationwidgetbase.ui \
         qdicomaddprinterwidgetbase.ui \
         qdicomprinterbasicsettingswidgetbase.ui
         
HEADERS +=  qdicomprintextension.h \
            dicomprintextensionmediator.h \
            qdicomprinterconfigurationwidget.h \
            dicomprintermanager.h \
            dicomprinter.h \
            dicomprintjob.h \
            dicomprintpage.h \
            dicomprint.h \
            createdicomprintspool.h \
            printdicomspool.h \
            qdicomaddprinterwidget.h \
            qdicomprinterbasicsettingswidget.h
            
SOURCES +=  qdicomprintextension.cpp \
            dicomprintextensionmediator.cpp \
            qdicomprinterconfigurationwidget.cpp \
            dicomprinter.cpp \
            dicomprintermanager.cpp \
            dicomprintjob.cpp \
            dicomprintpage.cpp \
            dicomprint.cpp \
            createdicomprintspool.cpp \
            printdicomspool.cpp \
            qdicomaddprinterwidget.cpp \
            qdicomprinterbasicsettingswidget.cpp

RESOURCES += dicomprint.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
