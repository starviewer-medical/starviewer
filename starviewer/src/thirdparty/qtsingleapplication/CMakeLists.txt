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

target_link_qt(qtsingleapplication
    Core
    Network
    Widgets
)
