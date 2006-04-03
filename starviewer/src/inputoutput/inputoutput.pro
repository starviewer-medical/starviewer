# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:  

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
           seriesvolum.h \
           studyvolum.h \
           cachepool.h \
           scaleimage.h \
           scalestudy.h \
           multiplequerystudy.h \
           qquerystudythread.h \
           operation.h \
           queueoperationlist.h \
           starviewerprocessimage.h \
           cacheinstallation.h \
           imagedicominformation.h 
SOURCES += input.cpp \
           output.cpp \
           cachepacs.cpp \
           databaseconnection.cpp \
           image.cpp \
           imagelist.cpp \
           imagelistsingleton.cpp \
           imagemask.cpp \
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
           seriesvolum.cpp \
           studyvolum.cpp \
           cachepool.cpp \
           scaleimage.cpp \
           scalestudy.cpp \
           multiplequerystudy.cpp \
           qquerystudythread.cpp \
           operation.cpp \
           queueoperationlist.cpp \
           starviewerprocessimage.cpp \
           cacheinstallation.cpp \
           imagedicominformation.cpp 
TARGETDEPS += ../tools/libtools.a \
../../src/repositories/librepositories.a
LIBS += ../../src/tools/libtools.a \
../../src/repositories/librepositories.a
INCLUDEPATH += ../../src/repositories \
../../src/tools
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += debug \
warn_on \
staticlib
TEMPLATE = lib
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
