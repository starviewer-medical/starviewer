set(SOURCES
    test_cachetest.cpp
    test_databaseconnection.cpp
    test_dicomdirburningapplicationtest.cpp
    test_dicommask.cpp
    test_echotopacs.cpp
    test_echotopacstest.cpp
    test_firewallaccesstest.cpp
    test_localdatabasebasedal.cpp
    test_pacsdevice.cpp
    test_pacsdevicemanager.cpp
    test_portinuse.cpp
    test_portinusetest.cpp
    test_qstudytreewidget.cpp
    test_senddicomfilestopacs.cpp
    test_upgradedatabasexmlparser.cpp
)

if(WIN32)
    list(APPEND SOURCES
        test_windowsfirewallaccess.cpp
    )
endif()

add_library(test_inputoutput OBJECT ${SOURCES})


################ Internal library dependencies ################

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput YES)
target_include_directories(test_inputoutput PRIVATE ${INPUTOUTPUT_INCLUDES})

target_link_libraries(test_inputoutput core inputoutput)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Network
    Sql
    Widgets
)
target_link_libraries(test_inputoutput Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets)

find_package(DCMTK REQUIRED)
target_include_directories(test_inputoutput SYSTEM PRIVATE
    ${DCMTK_INCLUDE_DIRS}/dcmtk/ofstd
)
target_link_libraries(test_inputoutput ${DCMTK_LIBRARIES})
