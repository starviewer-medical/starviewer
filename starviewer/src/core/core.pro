# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/core
# L'objectiu �s una biblioteca:  

FORMS += qlogviewerbase.ui \
         qcustomwindowleveldialogbase.ui 
TRANSLATIONS += core_ca_ES.ts \
                core_es_ES.ts \
                core_en_GB.ts 
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
           logging.h \
           deletedirectory.h \
           volume.h \
           volumerepository.h \
           identifier.h \
           repositorybase.h \
           repository.h \
           q2dviewer.h \
           q3dmprviewer.h \
           q3dviewer.h \
           qviewer.h \
           patient.h \
           series.h \
           study.h \
           parameters.h \
           qinputparameters.h \
           oldtool.h \
           output.h \
           q3dorientationmarker.h \
           slicing2dtool.h \
           oldtoolmanager.h \
           q2dviewertoolmanager.h \
           toolsactionfactory.h \
           oldzoomtool.h \
           zoomtool.h \
           oldtranslatetool.h \
           qlogviewer.h \
           roitool.h \
           distancetool.h \
           strokesegmentationmethod.h \
           itkErfcLevelSetFunction.h \
           itkErfcLevelSetImageFilter.h \
           itkVolumeCalculatorImageFilter.h \
           keyimagenotefilereader.h \
           keyimagenote.h \
           q2dviewerkeyimagenoteattacher.h \
           q2dviewerblackboard.h \
           vtkAtamaiPolyDataToImageStencil2.h \
           shutterfilter.h \
           q2dviewerpresentationstateattacher.h \
           image.h \
           dicomtagreader.h \
           line.h \
           drawingprimitive.h \
           olddrawer.h \
           text.h \
           patientfiller.h \
           patientfillerstep.h \
           patientfillerinput.h \
           dicomfileclassifierfillerstep.h \
           polygon.h \
           point.h \
           ellipse.h \
           ctfillerstep.h \
           imagefillerstep.h \
           presentationstatefillerstep.h \
           representation.h \
           oldtooldata.h \
           extensioncontext.h \
           keyimagenotefillerstep.h \
           dcmdatasetcache.h \
           distancerepresentation.h \
           temporaldimensionfillerstep.h \
           dicomimagereader.h \
           itkgdcmdicomimagereader.h \
           vtkdicomimagereader.h \
           dcmtkdicomimagereader.h \
           distancetooldata.h \
           volumegeneratorstep.h \
           patientbrowsermenu.h \
           patientbrowsermenubasicitem.h \
           patientbrowsermenuextendeditem.h \
           patientbrowsermenulist.h \
           referencelinesfillerstep.h \
           colorpalette.h \
           qcustomwindowleveldialog.h \
           qwindowlevelcombobox.h \
           mhdfileclassifierstep.h \
           orderimagesfillerstep.h \
           roitooldata.h \
           ellipserepresentation.h \
           toolregistry.h \
           toolmanager.h \
           toolconfiguration.h \
           tool.h \
           toolproxy.h \
           tooldata.h \
           referencelinestool.h \
           referencelinestooldata.h \
           imageplane.h \
           slicingtool.h \
           translatetool.h \
           oldwindowleveltool.h \
           oldvoxelinformationtool.h \
           oldscreenshottool.h \
           oldrotate3dtool.h \
           oldseedtool.h \
           voxelinformationtool.h \
           windowleveltool.h \
           seedtool.h \
           rotate3dtool.h \
           screenshottool.h \
           synchronizetool.h \
           synchronizetooldata.h \
           windowlevelpresetstool.h \
           drawer.h \
           drawerprimitive.h \
           drawerpolygon.h \
           drawerline.h \
           drawertext.h \
           drawerpolyline.h  \
           polylineroitooldata.h \
           polylineroitool.h
SOURCES += extensionmediator.cpp \
           extensionmanager.cpp \
           displayableid.cpp \
           mathtools.cpp \
           harddiskinformation.cpp \
           distance.cpp \
           deletedirectory.cpp \
           volume.cpp \
           volumerepository.cpp \
           identifier.cpp \
           repositorybase.cpp \
           repository.cpp \
           q2dviewer.cpp \
           q3dmprviewer.cpp \
           q3dviewer.cpp \
           qviewer.cpp \
           patient.cpp \
           series.cpp \
           study.cpp \
           parameters.cpp \
           qinputparameters.cpp \
           oldtool.cpp \
           output.cpp \
           q3dorientationmarker.cpp \
           slicing2dtool.cpp \
           oldtoolmanager.cpp \
           q2dviewertoolmanager.cpp \
           toolsactionfactory.cpp \
           oldzoomtool.cpp \
           zoomtool.cpp \
           oldtranslatetool.cpp \
           qlogviewer.cpp \
           roitool.cpp \
           distancetool.cpp \
           strokesegmentationmethod.cpp \
           itkErfcLevelSetFunction.cpp \
           itkErfcLevelSetImageFilter.cpp \
           itkVolumeCalculatorImageFilter.cpp \
           keyimagenote.cpp \
           keyimagenotefilereader.cpp \
           q2dviewerkeyimagenoteattacher.cpp \
           q2dviewerblackboard.cpp \
           vtkAtamaiPolyDataToImageStencil2.cxx \
           shutterfilter.cpp \
           q2dviewerpresentationstateattacher.cpp \
           image.cpp \
           dicomtagreader.cpp \
           line.cpp \
           drawingprimitive.cpp \
           olddrawer.cpp \
           text.cpp \
           patientfiller.cpp \
           patientfillerstep.cpp \
           patientfillerinput.cpp \
           dicomfileclassifierfillerstep.cpp \
           polygon.cpp \
           point.cpp \
           ellipse.cpp \
           ctfillerstep.cpp \
           imagefillerstep.cpp \
           presentationstatefillerstep.cpp \
           representation.cpp \
           oldtooldata.cpp \
           extensioncontext.cpp \
           keyimagenotefillerstep.cpp \
           dcmdatasetcache.cpp \
           distancerepresentation.cpp \
           temporaldimensionfillerstep.cpp \
           dicomimagereader.cpp \
           itkgdcmdicomimagereader.cpp \
           vtkdicomimagereader.cpp \
           dcmtkdicomimagereader.cpp \
           distancetooldata.cpp \
           volumegeneratorstep.cpp \
           patientbrowsermenu.cpp \
           patientbrowsermenubasicitem.cpp \
           patientbrowsermenuextendeditem.cpp \
           patientbrowsermenulist.cpp \
           referencelinesfillerstep.cpp \
           colorpalette.cpp \
           qcustomwindowleveldialog.cpp \
           qwindowlevelcombobox.cpp \
           mhdfileclassifierstep.cpp \
           orderimagesfillerstep.cpp \
           roitooldata.cpp \
           ellipserepresentation.cpp \
           toolregistry.cpp \
           toolmanager.cpp \
           toolconfiguration.cpp \
           tool.cpp \
           toolproxy.cpp \
           tooldata.cpp \
           referencelinestool.cpp \
           referencelinestooldata.cpp \
           imageplane.cpp \
           translatetool.cpp \
           slicingtool.cpp \
           oldwindowleveltool.cpp \
           oldvoxelinformationtool.cpp \
           oldscreenshottool.cpp \
           oldrotate3dtool.cpp \
           oldseedtool.cpp \
           voxelinformationtool.cpp \
           windowleveltool.cpp \
           seedtool.cpp \
           rotate3dtool.cpp \
           screenshottool.cpp \
           synchronizetool.cpp \
           synchronizetooldata.cpp \
           windowlevelpresetstool.cpp \
           drawer.cpp \
           drawerprimitive.cpp \
           drawerpolygon.cpp \
           drawerline.cpp \
           drawertext.cpp \
           drawerpolyline.cpp  \
           polylineroitool.cpp \
           polylineroitooldata.cpp
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
include(../dcmtk.inc)
include(../compilationtype.inc)
include(../translations.inc)
