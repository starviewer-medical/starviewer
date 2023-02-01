set(SOURCES
    diffusionperfusionsegmentationextensionmediator.cpp
    diffusionperfusionsegmentationsettings.cpp
    qdifuperfuextension.cpp
    volumecalculator.cpp
)

set(FORMS
    qdifuperfuextensionbase.ui
)

set(RESOURCES
    diffusionperfusionsegmentation.qrc
)

add_translations(QM_FILES diffusionperfusionsegmentation translations)

add_library(diffusionperfusionsegmentation ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(diffusionperfusionsegmentation)


################ Internal library dependencies ################

target_link_libraries(diffusionperfusionsegmentation core)


################ External library dependencies ################

target_link_qt(diffusionperfusionsegmentation
    Core
    Gui
    Widgets
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkImagingCore
    vtkIOImage
    vtkRenderingCore
)
target_link_libraries(diffusionperfusionsegmentation ${VTK_LIBRARIES})

