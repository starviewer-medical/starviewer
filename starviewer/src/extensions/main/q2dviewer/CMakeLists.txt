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

add_extension(q2dviewer)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(q2dviewer PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(q2dviewer PRIVATE ${INPUTOUTPUT_INCLUDES})

target_link_libraries(q2dviewer core inputoutput)


################ External library dependencies ################

target_link_qt(q2dviewer
    Core
    Gui
    Widgets
)
