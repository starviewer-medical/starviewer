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

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(rectumsegmentation PUBLIC ${CORE_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(rectumsegmentation Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
