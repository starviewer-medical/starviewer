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

add_translations(QM_FILES translations)

add_library(dicomprint ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(dicomprint PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(dicomprint PUBLIC ${INPUTOUTPUT_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(dicomprint Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})
target_link_libraries(dicomprint ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(dicomprint ${ITK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(dicomprint PUBLIC
    ${DCMTK_INCLUDE_DIRS}
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmimgle
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmpstat
)
target_link_libraries(dicomprint ${DCMTK_LIBRARIES})
