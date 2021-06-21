set(SOURCES
    angiosubstractionextensionmediator.cpp
    angiosubstractionsettings.cpp
    qangiosubstractionextension.cpp
)

set(FORMS
    qangiosubstractionextensionbase.ui
)

set(RESOURCES
    angiosubstraction.qrc
)

add_translations(QM_FILES angiosubstraction translations)

add_library(angiosubstraction ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(angiosubstraction PUBLIC ${CORE_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(angiosubstraction Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKOptimizers
    ITKTransform
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(angiosubstraction ${ITK_LIBRARIES})
