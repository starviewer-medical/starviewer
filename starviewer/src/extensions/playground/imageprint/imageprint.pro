# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/dicomprint
# L'objectiu � una biblioteca:  

FORMS += qimageprintextensionbase.ui \
	   qdicomjobcreatorwidgetbase.ui \
	   qdicomprinterconfigurationwidgetbase.ui \
	   qdicomprintingconfigurationwidgetbase.ui
HEADERS += qimageprintextension.h \					 
	     imageprintextensionmediator.h \
	     imageprintfactory.h \
	     dicomprintfactory.h \
 	     qprinterconfigurationwidget.h \
	     qdicomprinterconfigurationwidget.h \
  	     printerhandler.h \
 	     dicomprinterhandler.h \
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
	     fakeobjects.h 
SOURCES += qimageprintextension.cpp \					 		
           imageprintextensionmediator.cpp \
	     dicomprintfactory.cpp \
	     qdicomprinterconfigurationwidget.cpp \
	     dicomprinterhandler.cpp \
	     dicomprinter.cpp \
	     qdicomprintingconfigurationwidget.cpp \
	     dicomprinterhandler.cpp \
	     dicomprinter.cpp \
	     qdicomprintingconfigurationwidget.cpp \
	     qdicomjobcreatorwidget.cpp \
	     printjob.cpp \
	     dicomprintjob.cpp \
	     dicomprintmethod.cpp \
	     printpage.cpp \
	     dicomprintpage.cpp \
	     fakeobjects.cpp
	     
RESOURCES += imageprint.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
