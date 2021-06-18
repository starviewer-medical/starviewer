set(SOURCES
    applicationtranslationsloader.cpp
    crashhandler.cpp
    main.cpp
    syncactionsregister.cpp
)

set(RESOURCES
    main.qrc
)

add_translations(QM_FILES main)

add_executable(starviewer MACOSX_BUNDLE ${SOURCES} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(starviewer PRIVATE ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(starviewer PRIVATE ${INPUTOUTPUT_INCLUDES})

get_target_includes(INTERFACE_INCLUDES interface NO)
target_include_directories(starviewer PRIVATE ${INTERFACE_INCLUDES})

get_target_includes(BREAKPAD_INCLUDES breakpad NO)
target_include_directories(starviewer PRIVATE ${BREAKPAD_INCLUDES})

get_target_includes(EASYLOGGING_INCLUDES easylogging++ NO)
target_include_directories(starviewer PRIVATE ${EASYLOGGING_INCLUDES})

get_target_includes(QTSINGLEAPPLICATION_INCLUDES qtsingleapplication NO)
target_include_directories(starviewer PRIVATE ${QTSINGLEAPPLICATION_INCLUDES}/src)  # HACK because QtSingleApplication has a different structure

target_link_libraries(starviewer core inputoutput interface breakpad easylogging++ qtsingleapplication)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(starviewer Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
    vtkGUISupportQt
    vtkGUISupportQtOpenGL
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkIOImage
    vtkRenderingAnnotation
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingImage
    vtkRenderingOpenGL2
    vtkRenderingVolumeOpenGL2
)
include(${VTK_USE_FILE})
target_link_libraries(starviewer ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKIOGDCM
    ITKVTK
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(starviewer ${ITK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(starviewer PRIVATE
    ${DCMTK_INCLUDE_DIRS}
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmdata
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmjpeg
)
target_link_libraries(starviewer ${DCMTK_LIBRARIES})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
target_link_libraries(starviewer vtkgdcm)

target_link_libraries(starviewer "-framework Cocoa" "-framework OpenGL")


################ Other things ################

include(ApplicationsTargetNames)
include(InstallExtensions)
