execute_process(COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/usr/bin" "${CMAKE_CURRENT_BINARY_DIR}/usr/share/applications")

execute_process(COMMAND ln -s /opt/starviewer/starviewer.sh "${CMAKE_CURRENT_BINARY_DIR}/usr/bin/starviewer")

list(GET CPACK_BUILD_SOURCE_DIRS 0 SOURCE_DIR)
string(REPLACE " " "" OUTPUT_FILENAME ${CPACK_PACKAGE_NAME})    # "Starviewer" for "Starviewer" and "StarviewerLite" for "Starviewer Lite"
configure_file("${SOURCE_DIR}/installerresources/Starviewer.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/usr/share/applications/${OUTPUT_FILENAME}.desktop")
