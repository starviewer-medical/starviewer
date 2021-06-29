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

add_extension(angiosubstraction)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(angiosubstraction PUBLIC ${CORE_INCLUDES})

target_link_libraries(angiosubstraction core)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(angiosubstraction Qt5::Core Qt5::Gui Qt5::Widgets)

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
    ITKOptimizers
    ITKTransform
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
target_link_libraries(angiosubstraction ${ITK_LIBRARIES})
