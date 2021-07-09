set(SOURCES
    src/qtlocalpeer.cpp
    src/qtsingleapplication.cpp
)

add_library(qtsingleapplication SHARED ${SOURCES})
set_target_properties(qtsingleapplication PROPERTIES OUTPUT_NAME QtSolutions_SingleApplication)
target_include_directories(qtsingleapplication PUBLIC src)

if(WIN32)
    add_compile_definitions(QT_QTSINGLEAPPLICATION_EXPORT)
endif()


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Network
    Widgets
)
target_link_libraries(qtsingleapplication Qt5::Core Qt5::Network Qt5::Widgets)
