set(SOURCES
    starviewersapwrapper.cpp
    ../core/logging.cpp
    ../core/starviewerapplication.cpp
)

add_executable(starviewer_sapwrapper ${SOURCES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(starviewer_sapwrapper PRIVATE ${CORE_INCLUDES})

get_target_includes(EASYLOGGING_INCLUDES easylogging++ NO)
target_include_directories(starviewer_sapwrapper PRIVATE ${EASYLOGGING_INCLUDES})

target_link_libraries(starviewer_sapwrapper easylogging++)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)
target_link_libraries(starviewer_sapwrapper Qt5::Core Qt5::Widgets)
