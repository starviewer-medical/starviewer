# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:  

include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
TEMPLATE = lib
CONFIG += debug \
warn_on \
staticlib
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
OBJECTS_DIR = ../../tmp/obj
UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
INCLUDEPATH += ../../src/repositories \
../../src/tools
LIBS += ../../src/tools/libtools.a \
../../src/repositories/librepositories.a
TARGETDEPS += ../tools/libtools.a \
../../src/repositories/librepositories.a
SOURCES += input.cpp \
           output.cpp \
           cachepacs.cpp \
           databaseconnection.cpp \
           image.cpp \
           imagelist.cpp \
           imagelistsingleton.cpp \
           imagemask.cpp \
           mulquerystudy.cpp \
           pacsconnection.cpp \
           pacslist.cpp \
           pacslistdb.cpp \
           pacsnetwork.cpp \
           pacsparameters.cpp \
           pacsserver.cpp \
           procesimagesingleton.cpp \
           processimage.cpp \
           processimagesingleton.cpp \
           queryimagenumber.cpp \
           queryseries.cpp \
           querystudy.cpp \
           retrieveimages.cpp \
           retrievethreadslist.cpp \
           series.cpp \
           serieslist.cpp \
           serieslistsingleton.cpp \
           seriesmask.cpp \
           starviewersettings.cpp \
           status.cpp \
           study.cpp \
           studylistsingleton.cpp \
           studymask.cpp \
           studylist.cpp \
           retrievethread.cpp \
           seriesvolum.cpp \
           studyvolum.cpp \
           cachepool.cpp \
           scaleimage.cpp \
           scalestudy.cpp 
HEADERS += input.h \
           output.h \
           cachepacs.h \
           const.h \
           constkey.h \
           databaseconnection.h \
           image.h \
           imagelist.h \
           imagelistsingleton.h \
           imagemask.h \
           mulquerystudy.h \
           pacsconnection.h \
           pacslistdb.h \
           pacslist.h \
           pacsnetwork.h \
           pacsparameters.h \
           pacsserver.h \
           procesimagesingleton.h \
           processimage.h \
           processimagesingleton.h \
           queryimagenumber.h \
           queryseries.h \
           querystudy.h \
           retrieveimages.h \
           retrievethreadslist.h \
           series.h \
           serieslist.h \
           serieslistsingleton.h \
           seriesmask.h \
           starviewersettings.h \
           status.h \
           struct.h \
           study.h \
           studylistsingleton.h \
           studymask.h \
           studylist.h \
           retrievethread.h \
           seriesvolum.h \
           studyvolum.h \
           cachepool.h \
           scaleimage.h \
           scalestudy.h 
