set(SOURCES
    test_renderingstyle.cpp
)

add_library(test_q3dviewer OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_q3dviewer q3dviewer)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
)
target_link_libraries(test_q3dviewer Qt5::Core)
