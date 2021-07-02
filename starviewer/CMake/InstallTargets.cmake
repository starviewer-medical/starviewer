if(WIN32)
    # Targets
    install(TARGETS starviewer starviewer_crashreporter qtsingleapplication RUNTIME DESTINATION .)
    if(NOT STARVIEWER_LITE)
        install(TARGETS starviewer_sapwrapper RUNTIME DESTINATION .)
    endif()

    # Other files and directories
    install(DIRECTORY dicomdumpdefaulttags DESTINATION .)
    install(DIRECTORY hangingprotocols/Default/ DESTINATION hangingprotocols)
    if(USE_SPECIFIC_HANGING_PROTOCOLS)
        install(DIRECTORY ${DEPLOYMENT_REPO_DIR}/hangingprotocols/IDI/ DESTINATION hangingprotocols/Specific)
    endif()
    install(DIRECTORY releasenotes DESTINATION . FILES_MATCHING PATTERN "*.html" PATTERN "*.css" PATTERN "*.svg")
    install(FILES bin/log.conf bin/mkisofs.exe bin/Part3.xml DESTINATION .)
    get_property(PDF_MANUALS GLOBAL PROPERTY PDF_MANUALS)
    install(FILES ${PDF_MANUALS} DESTINATION .)
    list(FILTER PDF_MANUALS INCLUDE REGEX "\\[en\\]")
    foreach(ENGLISH_MANUAL ${PDF_MANUALS})
        # TODO use cmake_path from CMake 3.20
        string(REPLACE "${CMAKE_BINARY_DIR}/bin/" "" SPANISH_MANUAL "${ENGLISH_MANUAL}")
        string(REPLACE "[en]" "[es]" SPANISH_MANUAL "${SPANISH_MANUAL}")
        install(FILES ${ENGLISH_MANUAL} DESTINATION . RENAME ${SPANISH_MANUAL})
    endforeach()

    # Libraries
    # TODO all Qt dlls are only for release, names are different for debug
    set(PLUGINS
        iconengines/qsvgicon.dll
        imageformats/qgif.dll
        imageformats/qsvg.dll
        platforms/qwindows.dll
        sqldrivers/qsqlite.dll
        styles/qwindowsvistastyle.dll
    )
    foreach(PLUGIN ${PLUGINS})
        # TODO use cmake_path from CMake 3.20
        string(REGEX REPLACE "([a-z]+)/.+" "\\1" DESTINATION ${PLUGIN})
        install(FILES "${QT_DIR}/plugins/${PLUGIN}" DESTINATION "plugins/${DESTINATION}")
    endforeach()
    set(QMLS
        plugins.qmltypes
        qmldir
        qtquick2plugin.dll
    )
    foreach(QML ${QMLS})
        install(FILES "${QT_DIR}/qml/QtQuick.2/${QML}" DESTINATION qml/QtQuick.2)
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
    set(QT_DLLS
        Qt5Concurrent.dll
        Qt5Core.dll
        Qt5Gui.dll
        Qt5Network.dll
        Qt5OpenGL.dll
        Qt5Qml.dll
        Qt5Quick.dll
        Qt5QuickWidgets.dll
        Qt5Sql.dll
        Qt5Svg.dll
        Qt5Widgets.dll
        Qt5Xml.dll
        Qt5XmlPatterns.dll
    )
    foreach(DLL ${QT_DLLS})
        install(FILES "${QT_DIR}/bin/${DLL}" DESTINATION .)
    endforeach()
    set(DLLS
        # VTK
        vtkCommonColor-8.2.dll
        vtkCommonComputationalGeometry-8.2.dll
        vtkCommonCore-8.2.dll
        vtkCommonDataModel-8.2.dll
        vtkCommonExecutionModel-8.2.dll
        vtkCommonMath-8.2.dll
        vtkCommonMisc-8.2.dll
        vtkCommonSystem-8.2.dll
        vtkCommonTransforms-8.2.dll
        vtkDICOMParser-8.2.dll
        vtkdoubleconversion-8.2.dll
        vtkFiltersCore-8.2.dll
        vtkFiltersExtraction-8.2.dll
        vtkFiltersGeneral-8.2.dll
        vtkFiltersGeometry-8.2.dll
        vtkFiltersHybrid-8.2.dll
        vtkFiltersModeling-8.2.dll
        vtkFiltersSources-8.2.dll
        vtkFiltersStatistics-8.2.dll
        vtkfreetype-8.2.dll
        vtkglew-8.2.dll
        vtkGUISupportQt-8.2.dll
        vtkImagingColor-8.2.dll
        vtkImagingCore-8.2.dll
        vtkImagingFourier-8.2.dll
        vtkImagingGeneral-8.2.dll
        vtkImagingHybrid-8.2.dll
        vtkImagingMath-8.2.dll
        vtkImagingSources-8.2.dll
        vtkInteractionStyle-8.2.dll
        vtkInteractionWidgets-8.2.dll
        vtkIOCore-8.2.dll
        vtkIOImage-8.2.dll
        vtkIOLegacy-8.2.dll
        vtkjpeg-8.2.dll
        vtklz4-8.2.dll
        vtklzma-8.2.dll
        vtkmetaio-8.2.dll
        vtkpng-8.2.dll
        vtkRenderingAnnotation-8.2.dll
        vtkRenderingCore-8.2.dll
        vtkRenderingFreeType-8.2.dll
        vtkRenderingImage-8.2.dll
        vtkRenderingOpenGL2-8.2.dll
        vtkRenderingVolume-8.2.dll
        vtkRenderingVolumeOpenGL2-8.2.dll
        vtksys-8.2.dll
        vtktiff-8.2.dll
        vtkzlib-8.2.dll
        # GDCM
        gdcmcharls.dll
        gdcmCommon.dll
        gdcmDICT.dll
        gdcmDSED.dll
        gdcmexpat.dll
        gdcmIOD.dll
        gdcmjpeg8.dll
        gdcmjpeg12.dll
        gdcmjpeg16.dll
        gdcmMSFF.dll
        gdcmopenjp2.dll
        gdcmzlib.dll
        vtkgdcm.dll
        # ThreadWeaver
        KF5ThreadWeaver.dll
    )
    foreach(DLL ${DLLS})
        install(FILES "${SDK_INSTALL_DIR}/bin/${DLL}" DESTINATION .)
    endforeach()
endif()
