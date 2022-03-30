set(SOURCES
    crashreporter.cpp
    qcrashreporter.cpp
    ../core/coresettings.cpp
    ../core/defaultsettings.cpp
    ../core/logging.cpp
    ../core/persistentsettings.cpp
    ../core/settings.cpp
    ../core/settingsaccesslevelfilereader.cpp
    ../core/settingsinterface.cpp
    ../core/settingsparser.cpp
    ../core/settingsregistry.cpp
    ../core/starviewerapplication.cpp
    ../main/applicationtranslationsloader.cpp
)

set(FORMS
    qcrashreporterbase.ui
)

set(RESOURCES
    crashreporter.qrc
    ../main/themes/${THEME}/${THEME}.qrc
)

if(USE_QTCONF AND WIN32)
    list(APPEND RESOURCES
        ../main/qtconf/win/qtconf.qrc
    )
endif()

add_translations(QM_FILES crashreporter crashreporter)

add_executable(starviewer_crashreporter WIN32 ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(starviewer_crashreporter PRIVATE ${CORE_INCLUDES})

target_link_libraries(starviewer_crashreporter easylogging++)

add_dependencies(starviewer_crashreporter starviewer)   # needed to release main translations before compiling main.qrc


################ External library dependencies ################

target_link_qt(starviewer_crashreporter
    Core
    Gui
    Network
    Widgets
)


################ Other things ################

include(ApplicationsTargetNames)
