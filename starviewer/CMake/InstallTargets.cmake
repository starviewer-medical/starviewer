################ Targets ################

if(NOT APPLE)
    install(TARGETS starviewer starviewer_crashreporter qtsingleapplication RUNTIME DESTINATION .)
    if(NOT STARVIEWER_LITE)
        install(TARGETS starviewer_sapwrapper RUNTIME DESTINATION .)
    endif()
endif()


################ Other files and directories ################

install(DIRECTORY dicomdumpdefaulttags DESTINATION .)
install(DIRECTORY hangingprotocols/Default/ DESTINATION hangingprotocols)
if(USE_SPECIFIC_HANGING_PROTOCOLS)
    install(DIRECTORY ${DEPLOYMENT_REPO_DIR}/hangingprotocols/IDI/ DESTINATION hangingprotocols/Specific)
endif()
install(DIRECTORY releasenotes DESTINATION . FILES_MATCHING PATTERN "*.html" PATTERN "*.css" PATTERN "*.svg")
install(FILES bin/log.conf bin/Part3.xml DESTINATION .)
get_property(PDF_MANUALS GLOBAL PROPERTY PDF_MANUALS)
if(NOT STARVIEWER_CE)
    list(FILTER PDF_MANUALS EXCLUDE REGEX "How to access IFU")
endif()
install(FILES ${PDF_MANUALS} DESTINATION .)

if(WIN32)
    install(PROGRAMS bin/mkisofs.exe DESTINATION .)
elseif(UNIX AND NOT APPLE)
    install(PROGRAMS bin/starviewer.sh DESTINATION .)
    install(DIRECTORY src/main/themes/${THEME}/images/logo DESTINATION .)
endif()


################ Libraries ################

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(MSVC AND ${BUILD_TYPE} STREQUAL debug AND NOT STARVIEWER_BUILD_DEBUG_WITH_RELEASE_LIBS)
    set(DEBUG_LIBRARY_SUFFIX d)
else()
    set(DEBUG_LIBRARY_SUFFIX "")
endif()

set(PLUGINS
    iconengines/qsvgicon
    imageformats/qgif
    imageformats/qsvg
    sqldrivers/qsqlite
)
if(WIN32)
    list(APPEND PLUGINS
        platforms/qwindows
        styles/qwindowsvistastyle
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND PLUGINS
        platforminputcontexts/composeplatforminputcontextplugin
        platforminputcontexts/ibusplatforminputcontextplugin
        platforms/qwayland-egl
        platforms/qwayland-generic
        platforms/qwayland-xcomposite-egl
        platforms/qwayland-xcomposite-glx
        platforms/qxcb
        platformthemes/qgtk3
        wayland-decoration-client/bradient
        wayland-graphics-integration-client/dmabuf-server
        wayland-graphics-integration-client/drm-egl-server
        wayland-graphics-integration-client/qt-plugin-wayland-egl
        wayland-graphics-integration-client/shm-emulation-server
        wayland-graphics-integration-client/xcomposite-egl
        wayland-graphics-integration-client/xcomposite-glx
        wayland-graphics-integration-server/dmabuf-server
        wayland-graphics-integration-server/drm-egl-server
        wayland-graphics-integration-server/qt-plugin-wayland-egl
        wayland-graphics-integration-server/shm-emulation-server
        wayland-graphics-integration-server/wayland-eglstream-controller
        wayland-graphics-integration-server/xcomposite-egl
        wayland-graphics-integration-server/xcomposite-glx
        wayland-shell-integration/ivi-shell
        wayland-shell-integration/wl-shell
        wayland-shell-integration/xdg-shell
        wayland-shell-integration/xdg-shell-v5
        wayland-shell-integration/xdg-shell-v6
        xcbglintegrations/qxcb-egl-integration
        xcbglintegrations/qxcb-glx-integration
    )
endif()
foreach(PLUGIN ${PLUGINS})
    # TODO use cmake_path from CMake 3.20
    string(REGEX MATCH "([^/]+)/([^/]+)" _ ${PLUGIN})
    set(FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${CMAKE_MATCH_2}${DEBUG_LIBRARY_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(PROGRAMS "${QT_DIR}/plugins/${CMAKE_MATCH_1}/${FILENAME}" DESTINATION "plugins/${CMAKE_MATCH_1}")
endforeach()

set(QMLS
    plugins.qmltypes
    qmldir
    ${CMAKE_SHARED_LIBRARY_PREFIX}qtquick2plugin${DEBUG_LIBRARY_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
)
foreach(QML ${QMLS})
    install(PROGRAMS "${QT_DIR}/qml/QtQuick.2/${QML}" DESTINATION qml/QtQuick.2)
endforeach()

set(TRANSLATIONS
    qt_ca.qm
    qt_es.qm
    qtbase_ca.qm
    qtbase_es.qm
    qtmultimedia_ca.qm
    qtmultimedia_es.qm
    qtscript_ca.qm
    qtscript_es.qm
    qtxmlpatterns_ca.qm
    qtxmlpatterns_es.qm
)
foreach(TRANSLATION ${TRANSLATIONS})
    install(FILES "${QT_DIR}/translations/${TRANSLATION}" DESTINATION translations)
endforeach()

# TODO use install(RUNTIME_DEPENDENCY_SET ...) available since CMake 3.21 (not yet released at the time of writing)
# Qt
set(QT_LIBS
    Qt5Concurrent
    Qt5Core
    Qt5Gui
    Qt5Network
    Qt5OpenGL
    Qt5Qml
    Qt5Quick
    Qt5QuickWidgets
    Qt5Sql
    Qt5Svg
    Qt5Widgets
    Qt5Xml
    Qt5XmlPatterns
)
if(UNIX AND NOT APPLE)
    list(APPEND QT_LIBS
        Qt5DBus
        Qt5X11Extras
        Qt5XcbQpa
        icudata
        icui18n
        icuuc
    )
endif()
foreach(LIB ${QT_LIBS})
    set(FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${LIB}${DEBUG_LIBRARY_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX})
    if(WIN32)
        install(PROGRAMS "${QT_DIR}/bin/${FILENAME}" DESTINATION .)
    elseif(UNIX AND NOT APPLE)
        install(PROGRAMS "${QT_DIR}/lib/${FILENAME}" TYPE LIB)
        if(${LIB} MATCHES Qt5)
            install(PROGRAMS "${QT_DIR}/lib/${FILENAME}.5" TYPE LIB)
            install(PROGRAMS "${QT_DIR}/lib/${FILENAME}.5.12" TYPE LIB)
            install(PROGRAMS "${QT_DIR}/lib/${FILENAME}.5.12.6" TYPE LIB)
        elseif(${LIB} MATCHES icu)
            install(PROGRAMS "${QT_DIR}/lib/${FILENAME}.56" TYPE LIB)
            install(PROGRAMS "${QT_DIR}/lib/${FILENAME}.56.1" TYPE LIB)
        endif()
    endif()
endforeach()

# VTK
set(VTK_LIBS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonExecutionModel
    CommonMath
    CommonMisc
    CommonSystem
    CommonTransforms
    DICOMParser
    doubleconversion
    FiltersCore
    FiltersExtraction
    FiltersGeneral
    FiltersGeometry
    FiltersHybrid
    FiltersModeling
    FiltersSources
    FiltersStatistics
    freetype
    glew
    GUISupportQt
    ImagingColor
    ImagingCore
    ImagingFourier
    ImagingGeneral
    ImagingHybrid
    ImagingMath
    ImagingSources
    InteractionStyle
    InteractionWidgets
    IOCore
    IOImage
    IOLegacy
    jpeg
    lz4
    lzma
    metaio
    png
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingImage
    RenderingOpenGL2
    RenderingVolume
    RenderingVolumeOpenGL2
    sys
    tiff
    zlib
)
foreach(LIB ${VTK_LIBS})
    set(FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}vtk${LIB}-8.2${CMAKE_SHARED_LIBRARY_SUFFIX})
    if(WIN32)
        install(PROGRAMS "${SDK_INSTALL_DIR}/bin/${FILENAME}" DESTINATION .)
    elseif(UNIX AND NOT APPLE)
        install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}" TYPE LIB)
        install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.1" TYPE LIB)
    endif()
endforeach()

# GDCM
set(GDCM_LIBS
    gdcmcharls
    gdcmCommon
    gdcmDICT
    gdcmDSED
    gdcmexpat
    gdcmIOD
    gdcmjpeg8
    gdcmjpeg12
    gdcmjpeg16
    gdcmMSFF
    gdcmopenjp2
    gdcmzlib
    vtkgdcm
)
if(UNIX AND NOT APPLE)
    list(APPEND GDCM_LIBS
        gdcmuuid
    )
endif()
foreach(LIB ${GDCM_LIBS})
    set(FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX})
    if(WIN32)
        install(PROGRAMS "${SDK_INSTALL_DIR}/bin/${FILENAME}" DESTINATION .)
    elseif(UNIX AND NOT APPLE)
        install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}" TYPE LIB)
        if(${LIB} STREQUAL gdcmcharls)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.2" TYPE LIB)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.2.0" TYPE LIB)
        elseif(${LIB} STREQUAL gdcmexpat)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.2.0" TYPE LIB)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.2.0.0" TYPE LIB)
        elseif(${LIB} STREQUAL gdcmopenjp2)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.2.3.0" TYPE LIB)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.7" TYPE LIB)
        else()
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.3.0" TYPE LIB)
            install(PROGRAMS "${SDK_INSTALL_DIR}/lib/${FILENAME}.3.0.4" TYPE LIB)
        endif()
    endif()
endforeach()

# ThreadWeaver
set(FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}KF5ThreadWeaver${CMAKE_SHARED_LIBRARY_SUFFIX})
if(WIN32)
    install(PROGRAMS "${SDK_INSTALL_DIR}/bin/${FILENAME}" DESTINATION .)
elseif(UNIX AND NOT APPLE)
    if(EXISTS "${SDK_INSTALL_DIR}/lib64")
        set(LIBDIR lib64)
    elseif(EXISTS "${SDK_INSTALL_DIR}/lib/${CMAKE_LIBRARY_ARCHITECTURE}")
        set(LIBDIR lib/${CMAKE_LIBRARY_ARCHITECTURE})
    else()
        set(LIBDIR lib)
    endif()
    install(PROGRAMS "${SDK_INSTALL_DIR}/${LIBDIR}/${FILENAME}" TYPE LIB)
    install(PROGRAMS "${SDK_INSTALL_DIR}/${LIBDIR}/${FILENAME}.5" TYPE LIB)
    install(PROGRAMS "${SDK_INSTALL_DIR}/${LIBDIR}/${FILENAME}.5.64.0" TYPE LIB)
endif()

# OpenSSL
# TODO With a newer version of CMake (3.18) it could be directly downloaded and extracted from here.
#      However for now we assume the dlls are in the sdk-install folder.
if(WIN32)
    set(OPENSSL_LIBS
        libcrypto-1_1-x64
        libssl-1_1-x64
    )
    foreach(LIB ${OPENSSL_LIBS})
        set(FILENAME ${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX})
        install(PROGRAMS "${SDK_INSTALL_DIR}/bin/${FILENAME}" DESTINATION .)
    endforeach()
endif()
