# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/interface
# L'objectiu és una biblioteca:  

FORMS += qconfigurationscreenbase.ui \
         qmpr3d2dextensionbase.ui \
         qmpr3dextensionbase.ui \
         qmprextensionbase.ui \
         queryscreenbase.ui \
         qpacslistbase.ui \
         qstudytreewidgetbase.ui \
         qserieslistwidgetbase.ui \
         q2dviewerextensionbase.ui \
         qcustomwindowleveldialogbase.ui \
         qcreatedicomdirbase.ui \
         qstrokesegmentationextensionbase.ui \
         qlandmarkregistrationextensionbase.ui \
         qoperationstatescreenbase.ui \
         qedemasegmentationextensionbase.ui \
         qdifuperfuextensionbase.ui 
TRANSLATIONS += interface_ca_ES.ts \
                interface_es_ES.ts \
                interface_en_GB.ts 
HEADERS += qapplicationmainwindow.h \
           appimportfile.h \
           extensioncreator.h \
           extensionfactory2.h \
           extensionhandler.h \
           extensionworkspace.h \
           qconfigurationscreen.h \
           qpacslist.h \
           queryscreen.h \
           qmprextension.h \
           qmprextensioncreator.h \
           qmpr3dextensioncreator.h \
           qmpr3dextension.h \
           qmpr3d2dextensioncreator.h \
           qmpr3d2dextension.h \
           qstudytreewidget.h \
           qserieslistwidget.h \
           q2dviewerextension.h \
           q2dviewerextensioncreator.h \
           qcustomwindowleveldialog.h \
           qwindowlevelcombobox.h \
           qcreatedicomdir.h \
           qdifuperfuextension.h \
           itkRegistre3DAffine.h \
           udgPerfusionEstimator.h \
           udgBinaryMaker.h \
           qstrokesegmentationextension.h \
           strokesegmentationmethod.h \
           qlandmarkregistrationextension.h \
           reglandmark.h \
           landmarkregistrator.h \
           leastsquareddistancecostfunction.h \
           qoperationstatescreen.h \
           itkVolumeCalculatorImageFilter.h \
           qedemasegmentationextension.h \
           itkErfcLevelSetFunction.h \
           itkErfcLevelSetImageFilter.h 
SOURCES += qapplicationmainwindow.cpp \
           appimportfile.cpp \
           extensioncreator.cpp \
           extensionfactory2.cpp \
           extensionhandler.cpp \
           extensionworkspace.cpp \
           qconfigurationscreen.cpp \
           qpacslist.cpp \
           queryscreen.cpp \
           qmprextension.cpp \
           qmprextensioncreator.cpp \
           qmpr3dextensioncreator.cpp \
           qmpr3dextension.cpp \
           qmpr3d2dextensioncreator.cpp \
           qmpr3d2dextension.cpp \
           qstudytreewidget.cpp \
           qserieslistwidget.cpp \
           q2dviewerextension.cpp \
           q2dviewerextensioncreator.cpp \
           qcustomwindowleveldialog.cpp \
           qwindowlevelcombobox.cpp \
           qcreatedicomdir.cpp \
           qdifuperfuextension.cpp \
           itkRegistre3DAffine.cpp \
           udgPerfusionEstimator.cpp \
           udgBinaryMaker.cpp \
           qstrokesegmentationextension.cpp \
           strokesegmentationmethod.cpp \
           qlandmarkregistrationextension.cpp \
           reglandmark.cpp \
           landmarkregistrator.cpp \
           leastsquareddistancecostfunction.cpp \
           qoperationstatescreen.cpp \
           itkVolumeCalculatorImageFilter.cpp \
           qedemasegmentationextension.cpp \
           itkErfcLevelSetFunction.cpp \
           itkErfcLevelSetImageFilter.cpp 
TARGETDEPS += ../core/libcore.a \
../../src/inputoutput/libinputoutput.a
LIBS += ../inputoutput/libinputoutput.a \
../core/libcore.a
INCLUDEPATH += ../inputoutput \
../core
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += warn_on \
qt \
opengl \
thread \
x11 \
staticlib \
exceptions \
stl
TEMPLATE = lib
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../compilationtype.inc)
