set(SOURCES
    test_anatomicalplane.cpp
    test_applicationupdatechecker.cpp
    test_applicationversionchecker.cpp
    test_applicationversiontest.cpp
    test_areameasurecomputer.cpp
    test_automaticsynchronizationtooldata.cpp
    test_bodysurfaceareaformula.cpp
    test_bodysurfaceareaformulacalculator.cpp
    test_colortransferfunction.cpp
    test_computezspacingpostprocessor.cpp
    test_decaycorrectionfactorformula.cpp
    test_decaycorrectionfactorformulacalculator.cpp
    test_defaultmeasurementtypeselector.cpp
    test_diagnosistestresultwriter.cpp
    test_dicomformattedvaluesconverter.cpp
    test_dicomserviceresponsestatus.cpp
    test_dicomsource.cpp
    test_dicomtagreader.cpp
    test_dicomvaluerepresentationconverter.cpp
    test_displayshutter.cpp
    test_distancemeasurecomputer.cpp
    test_drawerbitmap.cpp
    test_drawerline.cpp
    test_drawerpolygon.cpp
    test_drawertext.cpp
    test_externalapplication.cpp
    test_filter.cpp
    test_filteroutput.cpp
    test_firewallaccesstest.cpp
    test_griditerator.cpp
    test_hangingprotocol.cpp
    test_hangingprotocolimagesetrestriction.cpp
    test_hangingprotocolimagesetrestrictionexpression.cpp
    test_hangingprotocolmanager.cpp
    test_image.cpp
    test_imagefillerstep.cpp
    test_imageorientation.cpp
    test_imageorientationoperationsmapper.cpp
    test_imageoverlay.cpp
    test_imageoverlayreader.cpp
    test_imageoverlayregionfinder.cpp
    test_imageplane.cpp
    test_leanbodymassformula.cpp
    test_leanbodymassformulacalculator.cpp
    test_machineidentifier.cpp
    test_machineinformation.cpp
    test_mammographyimagehelper.cpp
    test_mathtools.cpp
    test_measurementmanager.cpp
    test_opacitytransferfunction.cpp
    test_optimalviewersgridestimator.cpp
    test_orderimagesfillerstep.cpp
    test_orthogonalplane.cpp
    test_patient.cpp
    test_patientcomparer.cpp
    test_patientfillerinput.cpp
    test_patientorientation.cpp
    test_pixelspacing2d.cpp
    test_pixelspacingamenderpostprocessor.cpp
    test_preferredpixelspacingselector.cpp
    test_q2dviewer.cpp
    test_relativegeometrylayout.cpp
    test_roidata.cpp
    test_screen.cpp
    test_screenlayout.cpp
    test_series.cpp
    test_slicehandler.cpp
    test_sliceorientedvolumepixeldata.cpp
    test_standardizeduptakevaluebodysurfaceareaformulacalculator.cpp
    test_standardizeduptakevaluebodyweightformulacalculator.cpp
    test_standardizeduptakevalueleanbodymassformulacalculator.cpp
    test_study.cpp
    test_studylayoutconfigsettingsconverter.cpp
    test_syncactionsconfiguration.cpp
    test_systemrequirementstest.cpp
    test_temporaldimensionfillerstep.cpp
    test_transferfunction.cpp
    test_vector3double.cpp
    test_vector3float.cpp
    test_voilut.cpp
    test_voilutpresetstooldata.cpp
    test_volume.cpp
    test_volumefillerstep.cpp
    test_volumepixeldata.cpp
    test_volumepixeldataiterator.cpp
    test_volumepixeldatareaderfactory.cpp
    test_voxel.cpp
    test_vtkimagedatacreator.cpp
    test_windowlevel.cpp
)

if(WIN32)
    list(APPEND SOURCES
        test_windowsfirewallaccess.cpp
        test_windowssysteminformation.cpp
    )
endif()

add_library(test_core OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_core core inputoutput interface q2dviewer q3dviewer mpr2d)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Network
    OpenGL
)
target_link_libraries(test_core Qt5::Network Qt5::OpenGL)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
    vtkGUISupportQt
    vtkIOImage
    vtkImagingCore
    vtkInteractionWidgets
    vtkRenderingAnnotation
    vtkRenderingCore
    vtkRenderingImage
    vtkRenderingOpenGL2
    vtkRenderingVolumeOpenGL2
)
include(${VTK_USE_FILE})
target_link_libraries(test_core ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKIOGDCM
    ITKVTK
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(test_core ${ITK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(test_core SYSTEM PRIVATE
    ${DCMTK_INCLUDE_DIRS}
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmdata
)

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
target_link_libraries(test_core vtkgdcm)

if(APPLE)
    target_link_libraries(test_core "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(test_core GLU)
elseif(WIN32)
    target_link_libraries(test_core glu32.lib)
endif()
