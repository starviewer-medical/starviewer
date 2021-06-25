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

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(diffusionperfusionsegmentation PUBLIC ${CORE_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(diffusionperfusionsegmentation Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKStatistics
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(diffusionperfusionsegmentation ${ITK_LIBRARIES})
