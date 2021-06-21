set(SOURCES
    pdfextensionmediator.cpp
    qpdfextension.cpp
)

set(FORMS
    qpdfextensionbase.ui
)

set(RESOURCES
    pdf.qrc
)

add_translations(QM_FILES pdf translations)

add_library(pdf ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(pdf PUBLIC ${CORE_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(pdf Qt5::Core Qt5::Widgets)
