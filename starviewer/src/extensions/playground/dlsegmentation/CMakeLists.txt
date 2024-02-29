set(SOURCES
    dlsegmentationextensionmediator.cpp
    qdlsegmentationextension.cpp
)

set(FORMS
    qdlsegmentationextensionbase.ui
)

set(RESOURCES
    dlsegmentation.qrc
)

add_translations(QM_FILES dlsegmentation translations)

add_library(dlsegmentation ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})

add_extension(dlsegmentation)


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(dlsegmentation PUBLIC ${CORE_INCLUDES})

target_link_libraries(dlsegmentation core)


################ External library dependencies ################

target_link_qt(dlsegmentation
    Core
    Widgets
)
