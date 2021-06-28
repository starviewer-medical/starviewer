set(SOURCES
    src/qtlocalpeer.cpp
    src/qtsingleapplication.cpp
)

add_library(qtsingleapplication SHARED ${SOURCES})
set_target_properties(qtsingleapplication PROPERTIES OUTPUT_NAME QtSolutions_SingleApplication-head)

if(WIN32)
    add_compile_definitions(QT_QTSINGLEAPPLICATION_EXPORT)
endif()

find_package(Qt5 REQUIRED COMPONENTS
    Network
    Widgets
)
target_link_libraries(qtsingleapplication Qt5::Network Qt5::Widgets)
