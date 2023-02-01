if(NOT STARVIEWER_LITE)
    set(SOURCES
        starviewersapwrapper.cpp
        ../core/logging.cpp
        ../core/starviewerapplication.cpp
    )

    if(USE_QTCONF)
        if(WIN32)
            list(APPEND RESOURCES
                ../main/qtconf/win/qtconf.qrc
            )
        elseif(UNIX AND NOT APPLE)
            list(APPEND RESOURCES
                ../main/qtconf/linux/qtconf.qrc
            )
        endif()
    endif()

    add_executable(starviewer_sapwrapper ${SOURCES} ${RESOURCES})


    ################ Internal library dependencies ################

    get_target_includes(CORE_INCLUDES core NO)
    target_include_directories(starviewer_sapwrapper PRIVATE ${CORE_INCLUDES})

    target_link_libraries(starviewer_sapwrapper easylogging++)


    ################ External library dependencies ################

    target_link_qt(starviewer_sapwrapper
        Core
        Widgets
    )
endif()
