set(SOURCES
    mpr2dextensionmediator.cpp
    mprsettings.cpp
    qmprextension.cpp
)

set(FORMS
    qmprextensionbase.ui
)

set(RESOURCES
    mpr2d.qrc
)

add_translations(QM_FILES mpr2d translations)

add_library(mpr2d ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(mpr2d)


################ Internal library dependencies ################

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(mpr2d PRIVATE ${INPUTOUTPUT_INCLUDES})

target_link_libraries(mpr2d core inputoutput)


################ External library dependencies ################

target_link_qt(mpr2d
    Core
    Gui
    Widgets
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkCommonTransforms
    vtkFiltersSources
    vtkImagingCore
    vtkRenderingAnnotation
    vtkRenderingCore
)
target_link_libraries(mpr2d ${VTK_LIBRARIES})
