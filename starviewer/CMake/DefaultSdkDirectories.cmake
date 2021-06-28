if(WIN32)
    set(QT_LAST_DIR msvc2017_64)
elseif(APPLE)
    set(QT_LAST_DIR clang_64)
elseif(UNIX)
    set(QT_LAST_DIR gcc_64)
endif()

set(CMAKE_PREFIX_PATH
    ${CMAKE_SOURCE_DIR}/../sdk-install
    ${CMAKE_SOURCE_DIR}/../sdk-install/qt/5.12.6/${QT_LAST_DIR}
)
