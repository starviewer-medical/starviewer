set(SOURCES
    test_qmrusortedtoolbuttonwithmenu.cpp
)

add_library(test_q2dviewer OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_q2dviewer q2dviewer)
