set(SOURCES
    test_qmrusortedtoolbuttonwithmenu.cpp
)

add_library(test_q2dviewer OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_q2dviewer q2dviewer)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Widgets
)
target_link_libraries(test_q2dviewer Qt5::Widgets)
