if(APPLE)
    set(STARVIEWER_EXE ../../../$<TARGET_FILE_NAME:starviewer>)
    #set(CRASHREPORTER_EXE $<TARGET_FILE:starviewer_crashreporter>)
    set(CRASHREPORTER_EXE starviewer_crashreporter.app/Contents/MacOS/starviewer_crashreporter)
else()
    set(STARVIEWER_EXE $<TARGET_FILE_NAME:starviewer>)
    set(CRASHREPORTER_EXE $<TARGET_FILE:starviewer_crashreporter>)
endif()

file(GENERATE OUTPUT executablesnames.h CONTENT
"#ifndef EXECUTABLESNAMES_H
#define EXECUTABLESNAMES_H
#define STARVIEWER_EXE \"${STARVIEWER_EXE}\"
#define STARVIEWER_CRASH_REPORTER_EXE \"${CRASHREPORTER_EXE}\"
#endif
")
