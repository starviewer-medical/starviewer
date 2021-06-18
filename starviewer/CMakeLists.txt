cmake_minimum_required(VERSION 3.13)

project(Starviewer
    VERSION 1.1.0
    DESCRIPTION "Medical Imaging Software"
    HOMEPAGE_URL https://starviewer.org/
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

if(APPLE)
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

include(GetTargetIncludes)
include(Translations)

add_subdirectory(src)
#add_subdirectory(tests)

#add_translation_targets()
