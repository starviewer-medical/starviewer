# Adds Qt dependencies in general, taking into account if it must force the release version
function(link_qt)
    find_package(Qt5 REQUIRED COMPONENTS ${ARGN})
    list(TRANSFORM ARGN PREPEND Qt5:: OUTPUT_VARIABLE QT_TARGETS)
    link_libraries(${QT_TARGETS})

    if(MSVC AND STARVIEWER_BUILD_DEBUG_WITH_RELEASE_LIBS)
        foreach(QT_TARGET ${QT_TARGETS} Qt5::Gui Qt5::WinMain)
            if(TARGET ${QT_TARGET}) # Gui and WinMain might not exist in some calls
                set_target_properties(${QT_TARGET} PROPERTIES MAP_IMPORTED_CONFIG_DEBUG Release)
            endif()
        endforeach()
    endif()
endfunction()

# Adds Qt dependencies to the given target, taking into account if it must force the release version
function(target_link_qt TARGET)
    find_package(Qt5 REQUIRED COMPONENTS ${ARGN})
    list(TRANSFORM ARGN PREPEND Qt5:: OUTPUT_VARIABLE QT_TARGETS)
    target_link_libraries(${TARGET} ${QT_TARGETS})

    if(MSVC AND STARVIEWER_BUILD_DEBUG_WITH_RELEASE_LIBS)
        foreach(QT_TARGET ${QT_TARGETS} Qt5::Gui Qt5::WinMain)
            if(TARGET ${QT_TARGET}) # Gui and WinMain might not exist in some calls
                set_target_properties(${QT_TARGET} PROPERTIES MAP_IMPORTED_CONFIG_DEBUG Release)
            endif()
        endforeach()
    endif()
endfunction()
