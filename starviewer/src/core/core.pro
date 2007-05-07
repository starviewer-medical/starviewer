# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/core
# L'objectiu � una biblioteca:  

FORMS += qlogviewerbase.ui 
HEADERS += extensionfactory.h \
           genericfactory.h \
           singleton.h \
           genericsingletonfactoryregister.h \
           extensionfactoryregister.h \
           extensionmediator.h \
           extensionmanager.h \
           displayableid.h \
           extensionmediatorfactory.h \
           extensionmediatorfactoryregister.h \
           installextension.h \
           itkImageToVTKImageFilter.h \
           itkVTKImageToImageFilter.h \
           mathtools.h \
           itkQtAdaptor.h \
           harddiskinformation.h \
           distance.h \
           point.h \
           logging.h \
           deletedirectory.h \
           volume.h \
           volumerepository.h \
           identifier.h \
           repositorybase.h \
           repository.h \
           volumesourceinformation.h \
           q2dviewer.h \
           q3dmprviewer.h \
           q3dviewer.h \
           qviewer.h \
           patient.h \
           patientserie.h \
           patientstudy.h \
           parameters.h \
           qinputparameters.h \
           tool.h \
           input.h \
           output.h \
           q3dorientationmarker.h \
           slicing2dtool.h \
           toolmanager.h \
           q2dviewertoolmanager.h \
           windowleveltool.h \
           toolsactionfactory.h \
           seedtool.h \
           zoomtool.h \
           q3dviewertoolmanager.h \
           translatetool.h \
           q3dmprviewertoolmanager.h \
           qlogviewer.h \
           screenshottool.h \
           rotate3dtool.h \
           distancetool.h \
           volumecalculator.h \
           itkVolumeCalculatorImageFilter.h 
SOURCES += extensionmediator.cpp \
           extensionmanager.cpp \
           displayableid.cpp \
           mathtools.cpp \
           harddiskinformation.cpp \
           distance.cpp \
           point.cpp \
           deletedirectory.cpp \
           volume.cpp \
           volumerepository.cpp \
           identifier.cpp \
           repositorybase.cpp \
           repository.cpp \
           volumesourceinformation.cpp \
           q2dviewer.cpp \
           q3dmprviewer.cpp \
           q3dviewer.cpp \
           qviewer.cpp \
           patient.cpp \
           patientserie.cpp \
           patientstudy.cpp \
           parameters.cpp \
           qinputparameters.cpp \
           tool.cpp \
           input.cpp \
           output.cpp \
           q3dorientationmarker.cpp \
           slicing2dtool.cpp \
           toolmanager.cpp \
           q2dviewertoolmanager.cpp \
           windowleveltool.cpp \
           toolsactionfactory.cpp \
           seedtool.cpp \
           zoomtool.cpp \
           q3dviewertoolmanager.cpp \
           translatetool.cpp \
           q3dmprviewertoolmanager.cpp \
           qlogviewer.cpp \
           screenshottool.cpp \
           rotate3dtool.cpp \
           distancetool.cpp \
           volumecalculator.cpp \
           itkVolumeCalculatorImageFilter.cpp 
TEMPLATE = lib
CONFIG += warn_on \
thread \
staticlib \
qt \
stl
OBJECTS_DIR = ../../tmp/obj
UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
LIBS += -llog4cxx
INCLUDEPATH += /usr/include/log4cxx/
include(../itk.inc)
include(../vtk.inc)
include(../compilationtype.inc)
