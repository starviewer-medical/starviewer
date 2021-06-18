set(SOURCES
    appimportfile.cpp
    extensionhandler.cpp
    extensionworkspace.cpp
    interfacesettings.cpp
    qaboutdialog.cpp
    qapplicationmainwindow.cpp
    qceaboutcontent.cpp
    qconfigurationdialog.cpp
    qlicensedialog.cpp
    qmedicaldeviceinformationdialog.cpp
    starviewerapplicationcommandline.cpp
)

set(FORMS
    qaboutdialogbase.ui
    qceaboutcontentbase.ui
    qconfigurationdialogbase.ui
    qlicensedialogbase.ui
    qmedicaldeviceinformationdialogbase.ui
)

add_translations(QM_FILES interface)

add_library(interface ${SOURCES} ${FORMS} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(interface PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(interface PRIVATE ${INPUTOUTPUT_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(interface Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
)
include(${VTK_USE_FILE})
target_link_libraries(interface ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(interface ${ITK_LIBRARIES})

find_package(KF5ThreadWeaver REQUIRED)
target_link_libraries(interface KF5::ThreadWeaver)
