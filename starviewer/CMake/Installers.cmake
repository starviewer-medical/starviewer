set(CMAKE_INSTALL_UCRT_LIBRARIES ON)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)

include(InstallRequiredSystemLibraries)

# Common
set(CPACK_PACKAGE_VENDOR Starviewer)
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/installerresources\\\\package-icon.bmp")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_SOURCE_DIR}/installerresources/CPackProjectConfigFile.cmake")
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

set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/src/main/themes/${THEME}/images/logo/logo.ico")
set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/installerresources/wix-banner.bmp")
set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/installerresources/wix-dialog.bmp")
set(CPACK_WIX_PATCH_FILE "${CMAKE_SOURCE_DIR}/installerresources/wixshortcuts.xml")

# DEB
# TODO specify versions. Example: set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libc6 (< 2.4)")
# TODO add openssl dependency for WADO
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libatk1.0-0, libatk-bridge2.0-0, libatspi2.0-0, libblkid1, libbsd0, libc6, libcairo2, libcairo-gobject2, libdatrie1,
    libdbus-1-3, libdrm2, libegl1, libepoxy0, libexpat1, libffi7, libfontconfig1, libfreetype6, libfribidi0, libgcc-s1, libgcrypt20, libgdk-pixbuf2.0-0,
    libgl1, libglib2.0-0, libglu1-mesa, libglvnd0, libglx0, libgpg-error0, libgraphite2-3, libgtk-3-0, libharfbuzz0b, libice6, liblz4-1, liblzma5, libmount1,
    libopengl0, libpango-1.0-0, libpangocairo-1.0-0, libpangoft2-1.0-0, libpcre2-8-0, libpcre3, libpixman-1-0, libpng16-16, libselinux1, libsm6, libstdc++6,
    libsystemd0, libthai0, libuuid1, libwayland-client0, libwayland-cursor0, libwayland-egl1, libwayland-server0, libx11-6, libx11-xcb1, libxau6, libxcb1,
    libxcb-glx0, libxcb-render0, libxcb-shm0, libxcb-xkb1, libxcomposite1, libxcursor1, libxdamage1, libxdmcp6, libxext6, libxfixes3, libxi6, libxinerama1,
    libxkbcommon0, libxkbcommon-x11-0, libxrandr2, libxrender1, libxt6, zlib1g")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Starviewer Team <support@starviewer.udg.edu>")

include(CPack)
