# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/interface
# L'objectiu és una biblioteca:  

FORMS += mutualinformationinputparametersformbase.ui \
         qconfigurationscreenbase.ui \
         qmpr3d2dextensionbase.ui \
         qmpr3dextensionbase.ui \
         qmprextensionbase.ui \
         qmutualinformationtab.ui \
         qretrievescreenbase.ui \
         qtabaxisviewbase.ui \
         queryscreenbase.ui \
         qpacslistbase.ui \
         qnavigatewindowbase.ui \
         qstudytreewidgetbase.ui \
         qserieslistwidgetbase.ui \
         qdefaultviewerextensionbase.ui \
         qcustomwindowleveldialogbase.ui 
TRANSLATIONS += interface_ca_ES.ts \
                interface_es_ES.ts \
                interface_en_GB.ts 
HEADERS += director.h \
           mutualinformationdirector.h \
           mutualinformationinputparametersform.h \
           mutualinformationparameters.h \
           parameters.h \
           qapplicationmainwindow.h \
           qinputparameters.h \
           qviewer.h \
           q2dviewer.h \
           q3dviewer.h \
           qvolumerepositorycombobox.h \
           qtabaxisview.h \
           appimportfile.h \
           extensioncreator.h \
           extensionfactory.h \
           qtabaxisviewextensioncreator.h \
           extensionhandler.h \
           extensionworkspace.h \
           qconfigurationscreen.h \
           qpacslist.h \
           qretrievescreen.h \
           queryscreen.h \
           q3dmprviewer.h \
           qmprextension.h \
           qmprextensioncreator.h \
           qmpr3dextensioncreator.h \
           qmpr3dextension.h \
           qmpr3d2dextensioncreator.h \
           qmpr3d2dextension.h \
           distancetool.h \
           tool.h \
           qnavigatewindow.h \
           qstudytreewidget.h \
           qserieslistwidget.h \
           qdefaultviewerextension.h \
           qdefaultviewerextensioncreator.h \
           qcustomwindowleveldialog.h 
SOURCES += director.cpp \
           mutualinformationdirector.cpp \
           mutualinformationinputparametersform.cpp \
           mutualinformationparameters.cpp \
           parameters.cpp \
           qapplicationmainwindow.cpp \
           qinputparameters.cpp \
           qviewer.cpp \
           q2dviewer.cpp \
           q3dviewer.cpp \
           qvolumerepositorycombobox.cpp \
           qtabaxisview.cpp \
           appimportfile.cpp \
           extensioncreator.cpp \
           extensionfactory.cpp \
           qtabaxisviewextensioncreator.cpp \
           extensionhandler.cpp \
           extensionworkspace.cpp \
           qconfigurationscreen.cpp \
           qpacslist.cpp \
           qretrievescreen.cpp \
           queryscreen.cpp \
           queryscreenerror.cpp \
           queryscreenmove.cpp \
           q3dmprviewer.cpp \
           qmprextension.cpp \
           qmprextensioncreator.cpp \
           qmpr3dextensioncreator.cpp \
           qmpr3dextension.cpp \
           qmpr3d2dextensioncreator.cpp \
           qmpr3d2dextension.cpp \
           distancetool.cpp \
           tool.cpp \
           qnavigatewindow.cpp \
           qstudytreewidget.cpp \
           qserieslistwidget.cpp \
           qdefaultviewerextension.cpp \
           qdefaultviewerextensioncreator.cpp \
           qcustomwindowleveldialog.cpp 
TARGETDEPS += ../repositories/librepositories.a \
../registration/libregistration.a \
../tools/libtools.a \
../../src/inputoutput/libinputoutput.a
LIBS += ../../src/inputoutput/libinputoutput.a \
../repositories/librepositories.a \
../inputoutput/libinputoutput.a \
../registration/libregistration.a \
../tools/libtools.a
INCLUDEPATH += ../../src/inputoutput \
../tools \
../inputoutput \
../registration \
../repositories
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += debug \
warn_on \
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
