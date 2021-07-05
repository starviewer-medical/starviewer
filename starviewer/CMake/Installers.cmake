set(CMAKE_INSTALL_UCRT_LIBRARIES ON)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VENDOR Starviewer)
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/main/images/installers\\\\package-icon.bmp")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(STARVIEWER_LITE)
    set(CPACK_PACKAGE_NAME "Starviewer Lite")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Starviewer Lite")
    set(CPACK_PACKAGE_EXECUTABLES starviewer_lite "Starviewer Lite")
    set(CPACK_CREATE_DESKTOP_LINKS starviewer_lite)
else()
    set(CPACK_PACKAGE_NAME Starviewer)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY Starviewer)
    set(CPACK_PACKAGE_EXECUTABLES starviewer Starviewer)
    set(CPACK_CREATE_DESKTOP_LINKS starviewer)
endif()

set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/src/main/images/installers\\\\nsis-welcome.bmp")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_INSTALLED_ICON_NAME starviewer.exe)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
set(CPACK_NSIS_MENU_LINKS
    "[ca] User guide.pdf" "Manual d'usuari (ca)"
    "[en] User guide.pdf" "User manual (en)"
    "[es] User guide.pdf" "Manual de usuario (es)"
)

include(CPack)
