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

set(RESOURCES
    crashreporter.qrc
    ../main/main.qrc
)

add_translations(QM_FILES crashreporter crashreporter)

add_executable(starviewer_crashreporter ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(starviewer_crashreporter PRIVATE ${CORE_INCLUDES})

get_target_includes(EASYLOGGING_INCLUDES easylogging++ NO)
target_include_directories(starviewer_crashreporter PRIVATE ${EASYLOGGING_INCLUDES})

target_link_libraries(starviewer_crashreporter easylogging++)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Network
    Widgets
)
target_link_libraries(starviewer_crashreporter Qt5::Core Qt5::Network Qt5::Widgets)


################ Other things ################

include(ApplicationsTargetNames)
