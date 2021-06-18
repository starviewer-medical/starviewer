find_package(Qt5 REQUIRED COMPONENTS
    Core
    Network
    Widgets
)

set(SOURCES
    crashreporter.cpp
    qcrashreporter.cpp
    ../core/coresettings.cpp
    ../core/defaultsettings.cpp
    ../core/logging.cpp
    ../core/settings.cpp
    ../core/settingsaccesslevelfilereader.cpp
    ../core/settingsparser.cpp
    ../core/settingsregistry.cpp
    ../core/starviewerapplication.cpp
    ../main/applicationtranslationsloader.cpp
)

set(FORMS
    qcrashreporterbase.ui
)

add_executable(starviewer_crashreporter ${SOURCES} ${FORMS})

target_link_libraries(starviewer_crashreporter Qt5::Core Qt5::Network Qt5::Widgets)

target_include_directories(starviewer_crashreporter PRIVATE ../core ../main ../thirdparty/easylogging++)

#include(ApplicationsTargetNames)

add_translations(starviewer_crashreporter crashreporter)
