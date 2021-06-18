set(SOURCES
    mpr2dextensionmediator.cpp
    mprsettings.cpp
    qmprextension.cpp
)

set(FORMS
    qmprextensionbase.ui
)

set(RESOURCES
    mpr2d.qrc
)

add_translations(QM_FILES translations)

add_library(mpr2d ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(mpr2d PUBLIC ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(mpr2d PRIVATE ${INPUTOUTPUT_INCLUDES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(mpr2d Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})
target_link_libraries(mpr2d ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(mpr2d ${ITK_LIBRARIES})
