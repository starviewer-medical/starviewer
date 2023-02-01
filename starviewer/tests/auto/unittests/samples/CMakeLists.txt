set(SOURCES
    test_qstring.cpp
)

add_library(test_samples OBJECT ${SOURCES})


################ External library dependencies ################

target_link_qt(test_samples
    Core
)
