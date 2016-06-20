# Nom dels targets dels programes generats: Starviewer, el seu crashreporter i Starviewer_SapWrapper.

lite_version {
	TARGET_STARVIEWER = starviewer_lite
} else {
	TARGET_STARVIEWER = starviewer
}

TARGET_STARVIEWER_CRASH_REPORTER = starviewer_crashreporter

TARGET_STARVIEWER_SAP_WRAPPER = starviewer_sapwrapper


# Generate header file with the names of the executables

exesHeaderDir = $$OUT_PWD
INCLUDEPATH += $$OUT_PWD
exesHeader = executablesnames.h
exesDefine = EXECUTABLESNAMES_H

# Should be defineReplace, but it didn't work
defineTest(createOrTruncateExesHeader) {
    win32:system(echo. 2> $$exesHeaderDir\\$$exesHeader)
    unix:system(> $$exesHeaderDir/$$exesHeader)

    return(true)
}

# Should be defineReplace, but it didn't work
defineTest(appendToExesHeader) {
    string = $$1

    win32:system(echo $$string >> $$exesHeaderDir\\$$exesHeader)
    unix:system(echo \'$$string\' >> $$exesHeaderDir/$$exesHeader)

    return(true)
}

defineReplace(getDefineString) {
    return("$${LITERAL_HASH}define $$1 \"$$2\"")
}

macx {
    starviewerExe = ../../../$${TARGET_STARVIEWER}
}
linux* {
    starviewerExe = $${TARGET_STARVIEWER}
}
win32 {
    starviewerExe = $${TARGET_STARVIEWER}.exe
}

macx {
    crashReporterExe = $${TARGET_STARVIEWER_CRASH_REPORTER}.app/Contents/MacOS/$${TARGET_STARVIEWER_CRASH_REPORTER}
}
linux* {
    crashReporterExe = $${TARGET_STARVIEWER_CRASH_REPORTER}
}
win32 {
    crashReporterExe = $${TARGET_STARVIEWER_CRASH_REPORTER}.exe
}

createOrTruncateExesHeader()
appendToExesHeader("$${LITERAL_HASH}ifndef $$exesDefine")
appendToExesHeader("$${LITERAL_HASH}define $$exesDefine")
appendToExesHeader($$getDefineString(STARVIEWER_EXE, $$starviewerExe))
appendToExesHeader($$getDefineString(STARVIEWER_CRASH_REPORTER_EXE, $$crashReporterExe))
appendToExesHeader($${LITERAL_HASH}endif)
