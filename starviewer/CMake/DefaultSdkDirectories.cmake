if(WIN32)
    set(QT_LAST_DIR msvc2017_64)
elseif(APPLE)
    set(QT_LAST_DIR clang_64)
elseif(UNIX)
    set(QT_LAST_DIR gcc_64)
endif()

set(SDK_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../sdk-install)
set(QT_DIR ${SDK_INSTALL_DIR}/qt/5.12.6/${QT_LAST_DIR})

set(CMAKE_PREFIX_PATH
    ${SDK_INSTALL_DIR}
    ${QT_DIR}
)
