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

# Adds VTK dependencies to the given target
function(target_link_vtk TARGET)
    find_package(VTK REQUIRED COMPONENTS ${ARGN})
    list(TRANSFORM ARGN PREPEND VTK:: OUTPUT_VARIABLE VTK_TARGETS)
    target_link_libraries(${TARGET} ${VTK_TARGETS})
    vtk_module_autoinit(TARGETS ${TARGET} MODULES ${VTK_TARGETS})
endfunction()

# Adds DCMTK dependencies to the given target
function(target_link_dcmtk TARGET)
    find_package(DCMTK REQUIRED COMPONENTS ${ARGN})
    list(TRANSFORM ARGN PREPEND "${DCMTK_INCLUDE_DIRS}/dcmtk/" OUTPUT_VARIABLE DCMTK_INCLUDES)
    list(TRANSFORM ARGN PREPEND DCMTK:: OUTPUT_VARIABLE DCMTK_TARGETS)
    target_include_directories(${TARGET} SYSTEM PUBLIC "${DCMTK_INCLUDE_DIRS}/dcmtk" ${DCMTK_INCLUDES})
    target_link_libraries(${TARGET} ${DCMTK_TARGETS})
    # The following is needed only because of to ITKIODCMTK
    list(GET DCMTK_TARGETS 0 FIRST_DCMTK_TARGET)
    get_target_property(DCMTK_LIB_FILE ${FIRST_DCMTK_TARGET} LOCATION)
    string(REGEX MATCH "^(.*)/([^/]+)$" _ ${DCMTK_LIB_FILE})
    # Now CMAKE_MATCH_1 contains the DCMTK lib dir
    target_link_directories(${TARGET} PUBLIC ${CMAKE_MATCH_1})
endfunction()
