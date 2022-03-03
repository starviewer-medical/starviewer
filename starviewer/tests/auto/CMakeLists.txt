############### Unspecific library dependencies ###############

include_directories(shared)

link_qt(
    Core
    Test
    Widgets
)


########################### Targets ###########################

add_subdirectory(shared)
add_subdirectory(unittests)

set(SOURCES
    autotests.cpp
)

set(RESOURCES
    ../../src/main/main.qrc
)

add_executable(autotests ${SOURCES} ${RESOURCES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(autotests PRIVATE ${CORE_INCLUDES})

get_target_includes(EASYLOGGING_INCLUDES easylogging++ NO)
target_include_directories(autotests PRIVATE ${EASYLOGGING_INCLUDES})

target_link_libraries(autotests test_shared test_core test_inputoutput test_interface test_q2dviewer test_q3dviewer test_samples easylogging++)
