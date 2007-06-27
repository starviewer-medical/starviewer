# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:  

HEADERS += const.h \
           databaseconnection.h \
           image.h \
           imagelist.h \
           pacsconnection.h \
           pacslistdb.h \
           pacslist.h \
           pacsnetwork.h \
           pacsparameters.h \
           pacsserver.h \
           processimage.h \
           processimagesingleton.h \
           retrieveimages.h \
           series.h \
           serieslist.h \
           serieslistsingleton.h \
           starviewersettings.h \
           status.h \
           struct.h \
           dicomstudy.h \
           studylistsingleton.h \
           studylist.h \
           seriesvolum.h \
           studyvolum.h \
           cachepool.h \
           scaleimage.h \
           scalestudy.h \
           multiplequerystudy.h \
           qquerystudythread.h \
           operation.h \
           queueoperationlist.h \
           cacheinstallation.h \
           imagedicominformation.h \
           qexecuteoperationthread.h \
           cachelayer.h \
           cacheimagedal.h \
           cacheseriesdal.h \
           cachestudydal.h \
           converttodicomdir.h \
           convertdicomtolittleendian.h \
           createdicomdir.h \
           cachetools.h \
           readdicomdir.h \
           storeimages.h \
           starviewerprocessimagestored.h \
           starviewerprocessimageretrieved.h \
           imagelistsingleton.h \
           querypacs.h \
           dicommask.h \
           importdicomdir.h 
SOURCES += databaseconnection.cpp \
           image.cpp \
           imagelist.cpp \
           pacsconnection.cpp \
           pacslist.cpp \
           pacslistdb.cpp \
           pacsnetwork.cpp \
           pacsparameters.cpp \
           pacsserver.cpp \
           processimage.cpp \
           processimagesingleton.cpp \
           retrieveimages.cpp \
           series.cpp \
           serieslist.cpp \
           serieslistsingleton.cpp \
           starviewersettings.cpp \
           status.cpp \
           dicomstudy.cpp \
           studylistsingleton.cpp \
           studylist.cpp \
           seriesvolum.cpp \
           studyvolum.cpp \
           cachepool.cpp \
           scaleimage.cpp \
           scalestudy.cpp \
           multiplequerystudy.cpp \
           qquerystudythread.cpp \
           operation.cpp \
           queueoperationlist.cpp \
           cacheinstallation.cpp \
           imagedicominformation.cpp \
           qexecuteoperationthread.cpp \
           cachelayer.cpp \
           cacheimagedal.cpp \
           cacheseriesdal.cpp \
           cachestudydal.cpp \
           converttodicomdir.cpp \
           convertdicomtolittleendian.cpp \
           createdicomdir.cpp \
           cachetools.cpp \
           readdicomdir.cpp \
           storeimages.cpp \
           starviewerprocessimagestored.cpp \
           starviewerprocessimageretrieved.cpp \
           imagelistsingleton.cpp \
           querypacs.cpp \
           dicommask.cpp \
           importdicomdir.cpp 
TARGETDEPS += ../core/libcore.a
LIBS += ../../src/core/libcore.a
INCLUDEPATH += ../../src/core
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += warn_on \
staticlib
TEMPLATE = lib
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../compilationtype.inc)
