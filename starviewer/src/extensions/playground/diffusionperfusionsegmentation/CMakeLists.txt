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

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(diffusionperfusionsegmentation Qt5::Core Qt5::Gui Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkImagingCore
    vtkIOImage
    vtkRenderingCore
)
target_link_libraries(diffusionperfusionsegmentation ${VTK_LIBRARIES})

