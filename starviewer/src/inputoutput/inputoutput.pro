# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:  

TRANSLATIONS += inputoutput_ca_ES.ts \
                inputoutput_es_ES.ts \
                inputoutput_en_GB.ts 
FORMS += qconfigurationscreenbase.ui \
         qpacslistbase.ui \
         qstudytreewidgetbase.ui \
         qserieslistwidgetbase.ui \
         qcreatedicomdirbase.ui \
         qoperationstatescreenbase.ui \
         qchooseoneobjectdialogbase.ui \
         queryscreenbase.ui \
 qbasicsearchwidgetbase.ui
HEADERS += databaseconnection.h \
           dicomimage.h \
           pacsconnection.h \
           pacslistdb.h \
           pacsnetwork.h \
           pacsparameters.h \
           pacsserver.h \
           processimage.h \
           processimagesingleton.h \
           retrieveimages.h \
           dicomseries.h \
           starviewersettings.h \
           status.h \
           struct.h \
           dicomstudy.h \
           cachepool.h \
           scalestudy.h \
           multiplequerystudy.h \
           qquerystudythread.h \
           operation.h \
           queueoperationlist.h \
           cacheinstallation.h \
           qexecuteoperationthread.h \
           cachelayer.h \
           cacheimagedal.h \
           cacheseriesdal.h \
           cachestudydal.h \
           converttodicomdir.h \
           convertdicomtolittleendian.h \
           createdicomdir.h \
           cachetools.h \
           dicomdirreader.h \
           storeimages.h \
           starviewerprocessimagestored.h \
           starviewerprocessimageretrieved.h \
           querypacs.h \
           dicommask.h \
           dicomdirimporter.h \
           qconfigurationscreen.h \
           qpacslist.h \
           qstudytreewidget.h \
           qserieslistwidget.h \
           qcreatedicomdir.h \
           qoperationstatescreen.h \
           qchooseoneobjectdialog.h \
           queryscreen.h \
 	   errordcmtk.h \
 dcmdatasettostarviewerobject.h \
 qbasicsearchwidget.h
SOURCES += databaseconnection.cpp \
           dicomimage.cpp \
           pacsconnection.cpp \
           pacslistdb.cpp \
           pacsnetwork.cpp \
           pacsparameters.cpp \
           pacsserver.cpp \
           processimage.cpp \
           processimagesingleton.cpp \
           retrieveimages.cpp \
           dicomseries.cpp \
           starviewersettings.cpp \
           status.cpp \
           dicomstudy.cpp \
           cachepool.cpp \
           scalestudy.cpp \
           multiplequerystudy.cpp \
           qquerystudythread.cpp \
           operation.cpp \
           queueoperationlist.cpp \
           cacheinstallation.cpp \
           qexecuteoperationthread.cpp \
           cachelayer.cpp \
           cacheimagedal.cpp \
           cacheseriesdal.cpp \
           cachestudydal.cpp \
           converttodicomdir.cpp \
           convertdicomtolittleendian.cpp \
           createdicomdir.cpp \
           cachetools.cpp \
           dicomdirreader.cpp \
           storeimages.cpp \
           starviewerprocessimagestored.cpp \
           starviewerprocessimageretrieved.cpp \
           querypacs.cpp \
           dicommask.cpp \
           dicomdirimporter.cpp \
           qconfigurationscreen.cpp \
           qpacslist.cpp \
           qstudytreewidget.cpp \
           qserieslistwidget.cpp \
           qcreatedicomdir.cpp \
           qoperationstatescreen.cpp \
           qchooseoneobjectdialog.cpp \
           queryscreen.cpp \
 dcmdatasettostarviewerobject.cpp \
 qbasicsearchwidget.cpp

INCLUDEPATH += ../core
DEPENDPATH += ../core

TEMPLATE = lib

DESTDIR = ./

include(../corelibsconfiguration.inc)
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)
