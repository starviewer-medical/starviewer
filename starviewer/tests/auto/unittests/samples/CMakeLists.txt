set(SOURCES
    test_qstring.cpp
)

add_library(test_samples OBJECT ${SOURCES})


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
)
target_link_libraries(test_samples Qt5::Core)
