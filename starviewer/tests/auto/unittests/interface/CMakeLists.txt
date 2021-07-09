set(SOURCES
    test_qapplicationmainwindow.cpp
    test_starviewerapplicationcommandline.cpp
)

add_library(test_interface OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_interface core inputoutput interface)
