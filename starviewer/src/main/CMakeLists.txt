include(Theme)

set(SOURCES
    applicationtranslationsloader.cpp
    crashhandler.cpp
    main.cpp
    syncactionsregister.cpp
)

set(RESOURCES
    main.qrc
    ../qml/qml.qrc
    themes/${THEME}/${THEME}.qrc
)

if(WIN32)
    list(APPEND RESOURCES
        themes/${THEME}/${THEME}.rc
    )
endif()

if(STARVIEWER_CE)
    list(APPEND RESOURCES
        ../external/external.qrc
    )

    file(COPY ${DEPLOYMENT_REPO_DIR}/celabelsymbols DESTINATION ${CMAKE_SOURCE_DIR}/src/external)
endif()

if(USE_QTCONF)
    if(WIN32)
        list(APPEND RESOURCES
            qtconf/win/qtconf.qrc
        )
    elseif(UNIX AND NOT APPLE)
        list(APPEND RESOURCES
            qtconf/linux/qtconf.qrc
        )
    endif()
endif()

add_translations(QM_FILES main main)

# These must be included here for the dependencies of the custom commands to work
include(ReleaseNotes)
include(PdfManuals)

add_executable(starviewer WIN32 MACOSX_BUNDLE ${SOURCES} ${RESOURCES} ${QM_FILES} ${RELEASE_NOTES_DIR}/changelog.html ${PDF_MANUALS})


################ Internal library dependencies ################

target_link_libraries(starviewer core inputoutput interface breakpad easylogging++ qtsingleapplication)


################ External library dependencies ################

target_link_qt(starviewer
    Core
    Gui
    Widgets
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkGUISupportQt
)
target_link_libraries(starviewer ${VTK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(starviewer PRIVATE
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmdata
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmjpeg
)
target_link_libraries(starviewer ${DCMTK_LIBRARIES})


################ Other things ################

include(ApplicationsTargetNames)

set_target_properties(starviewer PROPERTIES OUTPUT_NAME ${STARVIEWER_NAME}) # Change name for Starviewer Lite

generate_extensions_header()
