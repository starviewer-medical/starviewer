set(CMAKE_INSTALL_UCRT_LIBRARIES ON)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)

include(InstallRequiredSystemLibraries)

# Common
set(CPACK_PACKAGE_VENDOR Starviewer)
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/installerresources\\\\package-icon.bmp")
set(CPACK_PACKAGE_CHECKSUM SHA256)
configure_file(LICENSE LICENSE.txt COPYONLY)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")  # WiX requires a RTF or TXT

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

# NSIS
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/installerresources\\\\nsis-welcome.bmp")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_INSTALLED_ICON_NAME starviewer.exe)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
set(CPACK_NSIS_MENU_LINKS
    "[ca] User guide.pdf" "Manual d'usuari (ca)"
    "[en] User guide.pdf" "User manual (en)"
    "[es] User guide.pdf" "Manual de usuario (es)"
)

# WiX
if(STARVIEWER_LITE)
    set(CPACK_WIX_UPGRADE_GUID E34A4959-E0E4-4E4E-978A-E9D1EE764937)
else()
    set(CPACK_WIX_UPGRADE_GUID 86A85188-E570-467A-B5CB-2F7691311436)
endif()

set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/src/main/images/logo/logo.ico")
set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/installerresources/wix-banner.bmp")
set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/installerresources/wix-dialog.bmp")
set(CPACK_WIX_PATCH_FILE "${CMAKE_SOURCE_DIR}/installerresources/wixshortcuts.xml")

include(CPack)
