set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)

include(InstallExtensions)

add_subdirectory(thirdparty)

add_subdirectory(core)
add_subdirectory(inputoutput)
add_subdirectory(interface)

add_subdirectory(extensions)

add_subdirectory(main)

if(APPLE)
    include(ApplicationsTargetNames)    # needed to know the actual name (different for Starviewer Lite)
    # In macOS, sapwrapper and crashreporter will go besides starviewer in the app bundle
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin/${STARVIEWER_NAME}.app/Contents/MacOS)
endif()

add_subdirectory(crashreporter)
add_subdirectory(starviewersapwrapper)
