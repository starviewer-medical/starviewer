cmake_minimum_required(VERSION 3.16)

project(Starviewer
    VERSION 1.1.0
    DESCRIPTION "Medical Imaging Software"
    HOMEPAGE_URL https://starviewer.org/
    LANGUAGES C CXX)

# Intended to represent the real version number with suffix (e.g. beta2) (except CE, that is added automatically)
set(STARVIEWER_VERSION 2.0.0-devel)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

include(Options)
include(CacheVariables)
include(BuildSettings)
include(DefaultSdkDirectories)
include(Theme)

include(GetTargetIncludes)
include(LinkQt)
include(Translations)

add_subdirectory(src)
add_subdirectory(tests)

add_translation_targets()

include(InstallTargets)
include(Installers)
