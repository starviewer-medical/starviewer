set(SOURCES
    q3dviewerextension.cpp
    q3dviewerextensionmediator.cpp
    q3dviewerextensionsettings.cpp
    renderingstyle.cpp
)

set(FORMS
    q3dviewerextensionbase.ui
)

set(RESOURCES
    q3dviewer.qrc
)

add_translations(QM_FILES q3dviewer translations)

add_library(q3dviewer ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(q3dviewer)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(q3dviewer PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(q3dviewer PRIVATE ${INPUTOUTPUT_INCLUDES})

target_link_libraries(q3dviewer core inputoutput)


################ External library dependencies ################

target_link_qt(q3dviewer
    Core
    Gui
    Widgets
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
    vtkRenderingCore
)
target_link_libraries(q3dviewer ${VTK_LIBRARIES})
