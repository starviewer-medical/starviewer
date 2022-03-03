set(SOURCES
    appimportfile.cpp
    extensionhandler.cpp
    extensionworkspace.cpp
    interfacesettings.cpp
    qaboutdialog.cpp
    qapplicationmainwindow.cpp
    qconfigurationdialog.cpp
    qlicensedialog.cpp
    starviewerapplicationcommandline.cpp
)

set(FORMS
    qaboutdialogbase.ui
    qconfigurationdialogbase.ui
    qlicensedialogbase.ui
)

if(STARVIEWER_CE)
    list(APPEND SOURCES
        qceaboutcontent.cpp
        qmedicaldeviceinformationdialog.cpp
    )
    list(APPEND FORMS
        qceaboutcontentbase.ui
        qmedicaldeviceinformationdialogbase.ui
    )
endif()

add_translations(QM_FILES interface interface)

add_library(interface ${SOURCES} ${FORMS} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(interface PRIVATE ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(interface PRIVATE ${INPUTOUTPUT_INCLUDES})

target_link_libraries(interface core inputoutput)


################ External library dependencies ################

target_link_qt(interface
    Core
    Gui
    Widgets
)

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
target_link_libraries(interface ${ITK_LIBRARIES})
