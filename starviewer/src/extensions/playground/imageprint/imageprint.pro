# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/dicomprint
# L'objectiu � una biblioteca:

FORMS += qimageprintextensionbase.ui \
         qdicomjobcreatorwidgetbase.ui \
         qdicomprinterconfigurationwidgetbase.ui \
         qdicomprintingconfigurationwidgetbase.ui \
         qdicomaddprinterwizardbase.ui
HEADERS +=  qimageprintextension.h \
            imageprintextensionmediator.h \
            imageprintfactory.h \
            dicomprintfactory.h \
            qprinterconfigurationwidget.h \
            qdicomprinterconfigurationwidget.h \
            printermanager.h \
            dicomprintermanager.h \
            printer.h \
            dicomprinter.h \
            qprintingconfigurationwidget.h \
            qdicomprintingconfigurationwidget.h \
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
            imageprintsettings.h \
            qdicomaddprinterwizard.h 
SOURCES +=  qimageprintextension.cpp \
            imageprintextensionmediator.cpp \
            dicomprintfactory.cpp \
            qdicomprinterconfigurationwidget.cpp \
            dicomprinter.cpp \
            qdicomprintingconfigurationwidget.cpp \
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
            imageprintsettings.cpp \
            qdicomaddprinterwizard.cpp

RESOURCES += imageprint.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
