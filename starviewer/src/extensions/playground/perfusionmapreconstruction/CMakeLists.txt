set(SOURCES
    perfusionmapcalculatormainthread.cpp
    perfusionmapcalculatorthread.cpp
    perfusionmapreconstructionextensionmediator.cpp
    perfusionmapreconstructionsettings.cpp
    qgraphicplotwidget.cpp
    qperfusionmapreconstructionextension.cpp
)

set(FORMS
    qgraphicplotwidgetbase.ui
    qperfusionmapreconstructionextensionbase.ui
)

set(RESOURCES
    perfusionmapreconstruction.qrc
)

add_translations(QM_FILES perfusionmapreconstruction translations)

add_library(perfusionmapreconstruction ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(perfusionmapreconstruction)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(perfusionmapreconstruction PUBLIC ${CORE_INCLUDES})

target_link_libraries(perfusionmapreconstruction core)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(perfusionmapreconstruction Qt5::Core Qt5::Gui Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
)
target_link_libraries(perfusionmapreconstruction ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKIOImageBase
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
target_link_libraries(perfusionmapreconstruction ${ITK_LIBRARIES})
