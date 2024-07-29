set(SOURCES
    qrectumsegmentationextension.cpp
    rectumsegmentationextensionmediator.cpp
    rectumSegmentationMethod.cpp
    rectumsegmentationsettings.cpp
)

set(FORMS
    qrectumsegmentationextensionbase.ui
)

set(RESOURCES
    rectumsegmentation.qrc
)

add_translations(QM_FILES rectumsegmentation translations)

add_library(rectumsegmentation ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(rectumsegmentation)


################ Internal library dependencies ################

target_link_libraries(rectumsegmentation core)


################ External library dependencies ################

target_link_qt(rectumsegmentation
    Core
    Widgets
)

target_link_vtk(rectumsegmentation
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersSources
    ImagingCore
    IOImage
    RenderingCore
)

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKIOImageBase
    ITKStatistics
    ITKTransform
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
target_link_libraries(rectumsegmentation ${ITK_LIBRARIES})
