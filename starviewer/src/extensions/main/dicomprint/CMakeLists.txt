set(SOURCES
    createdicomprintspool.cpp
    dicomprint.cpp
    dicomprinter.cpp
    dicomprintermanager.cpp
    dicomprintextensionmediator.cpp
    dicomprintjob.cpp
    dicomprintpage.cpp
    dicomprintpresentationstateimage.cpp
    printdicomspool.cpp
    qdicomaddprinterwidget.cpp
    qdicomprinterbasicsettingswidget.cpp
    qdicomprinterconfigurationwidget.cpp
    qdicomprintextension.cpp
)

set(FORMS
    qdicomaddprinterwidgetbase.ui
    qdicomprinterbasicsettingswidgetbase.ui
    qdicomprinterconfigurationwidgetbase.ui
    qdicomprintextensionbase.ui
)

set(RESOURCES
    dicomprint.qrc
)

add_translations(QM_FILES dicomprint translations)

add_library(dicomprint ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(dicomprint)


################ Internal library dependencies ################

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(dicomprint PUBLIC ${INPUTOUTPUT_INCLUDES})

target_link_libraries(dicomprint core inputoutput)


################ External library dependencies ################

target_link_qt(dicomprint
    Core
    Gui
    Widgets
)

target_link_dcmtk(dicomprint
    dcmpstat
)
