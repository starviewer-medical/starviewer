if(STARVIEWER_LITE)
    set(STARVIEWER_NAME starviewer_lite)
else()
    set(STARVIEWER_NAME starviewer)
endif()

set(STARVIEWER_EXE ${STARVIEWER_NAME}${CMAKE_EXECUTABLE_SUFFIX})
set(CRASHREPORTER_EXE starviewer_crashreporter${CMAKE_EXECUTABLE_SUFFIX})

add_compile_definitions(STARVIEWER_EXE=\"${STARVIEWER_EXE}\")
add_compile_definitions(STARVIEWER_CRASH_REPORTER_EXE=\"${CRASHREPORTER_EXE}\")
