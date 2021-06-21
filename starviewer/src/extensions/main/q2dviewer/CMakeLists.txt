set(SOURCES
    layoutmanager.cpp
    q2dviewerextension.cpp
    q2dviewerextensionmediator.cpp
    q2dviewersettings.cpp
    qhangingprotocolsgroupwidget.cpp
    qhangingprotocolswidget.cpp
    qmrusortedtoolbuttonwithmenu.cpp
    resetviewtoanatomicalplaneqviewercommand.cpp
    tablemenu.cpp
)

set(FORMS
    q2dviewerextensionbase.ui
    qhangingprotocolsgroupwidgetbase.ui
    qhangingprotocolswidgetbase.ui
)

set(RESOURCES
    q2dviewer.qrc
)

add_translations(QM_FILES q2dviewer translations)

add_library(q2dviewer ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(q2dviewer PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(q2dviewer PRIVATE ${INPUTOUTPUT_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
target_link_libraries(q2dviewer Qt5::Core Qt5::Gui Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})
target_link_libraries(q2dviewer ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(q2dviewer ${ITK_LIBRARIES})

find_package(KF5ThreadWeaver REQUIRED)
target_link_libraries(q2dviewer KF5::ThreadWeaver)
