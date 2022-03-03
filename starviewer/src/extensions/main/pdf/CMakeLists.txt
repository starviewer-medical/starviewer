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

target_link_qt(pdf
    Core
    Gui
    Widgets
)
