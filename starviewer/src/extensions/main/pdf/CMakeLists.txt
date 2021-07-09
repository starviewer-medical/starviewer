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

add_extension(pdf)


################ Internal library dependencies ################

target_link_libraries(pdf core)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(pdf Qt5::Core Qt5::Gui Qt5::Widgets)
