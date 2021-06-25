set(SOURCES
    exampleextensionmediator.cpp
    qexampleextension.cpp
)

set(FORMS
    qexampleextensionbase.ui
)

set(RESOURCES
    example.qrc
)

add_translations(QM_FILES example translations)

add_library(example ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(example)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(example PUBLIC ${CORE_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(example Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})
