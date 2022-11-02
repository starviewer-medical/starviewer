# TODO This won't work properly for Starviewer Lite because starviewer.sh is still hardcoded pointing to full Starviewer

execute_process(COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/usr/bin" "${CMAKE_CURRENT_BINARY_DIR}/usr/share/applications")

execute_process(COMMAND ln -s /opt/starviewer/starviewer.sh "${CMAKE_CURRENT_BINARY_DIR}/usr/bin/starviewer")

list(GET CPACK_BUILD_SOURCE_DIRS 0 SOURCE_DIR)
string(REPLACE " " "" OUTPUT_FILENAME ${CPACK_PACKAGE_NAME})    # "Starviewer" for "Starviewer" and "StarviewerLite" for "Starviewer Lite"
configure_file("${SOURCE_DIR}/installerresources/Starviewer.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/usr/share/applications/${OUTPUT_FILENAME}.desktop")

# URL handler
configure_file("${SOURCE_DIR}/installerresources/StarviewerUrlHandler.desktop.in"
               "${CMAKE_CURRENT_BINARY_DIR}/usr/share/applications/${OUTPUT_FILENAME}UrlHandler.desktop")
