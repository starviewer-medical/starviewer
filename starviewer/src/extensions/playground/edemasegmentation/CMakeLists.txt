set(SOURCES
    edemasegmentationextensionmediator.cpp
    edemasegmentationsettings.cpp
    qedemasegmentationextension.cpp
)

set(FORMS
    qedemasegmentationextensionbase.ui
)

set(RESOURCES
    edemasegmentation.qrc
)

add_translations(QM_FILES edemasegmentation translations)

add_library(edemasegmentation ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(edemasegmentation)


################ Internal library dependencies ################

target_link_libraries(edemasegmentation core)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(edemasegmentation Qt5::Core Qt5::Gui Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkImagingCore
    vtkIOImage
)
target_link_libraries(edemasegmentation ${VTK_LIBRARIES})
