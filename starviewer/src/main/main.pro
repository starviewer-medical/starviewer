# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/main
# L'objectiu és una aplicaci??:  ../../bin/starviewer

include(../vtk.inc)
include(../itk.inc)
TEMPLATE = app
CONFIG += debug \
warn_on
TARGET = ../../bin/starviewer
OBJECTS_DIR = ../../tmp/obj
UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
INCLUDEPATH += ../../src/inputoutput \
../main \
../filters \
../interface \
../colour \
../tools \
../inputoutput \
../visualization \
../registration \
../segmentation \
../repositories
LIBS += ../../src/inputoutput/libinputoutput.a \
-llog4cpp \
../interface/libinterface.a \
../repositories/librepositories.a \
../inputoutput/libinputoutput.a \
../filters/libfilters.a \
../colour/libcolour.a \
../tools/libtools.a \
../visualization/libvisualization.a \
../registration/libregistration.a \
../segmentation/libsegmentation.a
TARGETDEPS += ../interface/libinterface.a \
../repositories/librepositories.a \
../inputoutput/libinputoutput.a \
../filters/libfilters.a \
../colour/libcolour.a \
../tools/libtools.a \
../visualization/libvisualization.a \
../registration/libregistration.a \
../segmentation/libsegmentation.a \
../../src/inputoutput/libinputoutput.a
include(../dcmtk.inc)
SOURCES += main.cpp 
IMAGES += images/icon.png \
          images/new.png \
          images/open.png \
          images/save.png \
          images/cut.png \
          images/copy.png \
          images/paste.png \
          images/delete.png \
          images/find.png \
          images/gotocell.png \
		  images/splash.png \
          images/airbrush.png \
          images/debug.png \
          images/exit.png \
          images/fileclose.png \
          images/find.png \
          images/view_left_right.png \
          images/view_top_bottom.png \
          images/window_new.png \
          images/folderclose.png \
          images/folderopen.png \
          images/series.png 
